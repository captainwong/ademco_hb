#ifndef _WIN32
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#else
# ifndef  _CRT_SECURE_NO_WARNINGS
#  define  _CRT_SECURE_NO_WARNINGS
# endif
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#endif

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <thread>
#include <vector>
#include <mutex>
#include <string>
#include <algorithm>
#include <chrono>
#include <unordered_map>

#include <event2/listener.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/thread.h>

#ifdef ENABLE_BREAKPAD
#ifdef _WIN32
#include <client/windows/handler/exception_handler.h>
static bool dumpCallback(const wchar_t* dump_path,
						 const wchar_t* minidump_id,
						 void* context,
						 EXCEPTION_POINTERS* exinfo,
						 MDRawAssertionInfo* assertion,
						 bool succeeded)
{
	printf("Dump path: %ls\n", dump_path);
	return succeeded;
}

#else
#include <client/linux/handler/exception_handler.h>
static bool dumpCallback(const google_breakpad::MinidumpDescriptor& descriptor,
						 void* context, bool succeeded)
{
	printf("Dump path: %s\n", descriptor.path());
	return succeeded;
}
#endif
#endif


#define DISABLE_JLIB_LOG2
#include <ademco_packet.h>

using namespace ademco;

struct Client {
	int fd = 0;
	evbuffer* output = nullptr;
	std::string acct = {};
	size_t ademco_id = 0;
	uint16_t seq = 0;
	int type = -1;
};

struct ThreadContext {
	int worker_id = -1;
	event_base* base = nullptr;
	std::mutex mutex = {};
	std::unordered_map<int, Client> clients = {};
	AdemcoPacket packet = {};
};

event_base* listen_thread_evbase = nullptr;
int thread_count = 4;
std::vector<ThreadContext*> worker_thread_contexts = {};

// events will be sent to all clients
std::vector<ADEMCO_EVENT> events = {};
int threads_to_handled_event = 0;
std::mutex mutex = {};
struct UserInput {
	char pwd[1024] = { 0 };
	int ademco_id = 0;
	int ev = 0;
	int gg = 0;
	int zone = 0;
	char xdata[1024] = { 0 };
}userInput = {};


int disable_data_print = 0;

void op_usage()
{
	printf("Press a key and hit <enter>:\n"
		   "A: Arm\n"
		   "D: Disarm, require 6 digits password\n"
		   "E: Emergency\n"
		   "T: Query machine type\n"
		   "M: Mannualy input [event gg zone], Exampel Input: 'M' <enter> 3400 1 0 <enter>\n"
		   "C: Like M, not send to all clients, but send to specific client with ademco_id: [ademco_id event gg zone]\n"
		   "X: Like C, with xdata: [ademco_id event gg zone xdata]\n"
		   "\n"
		   "I: Print clients info\n"
		   "P: Toggle enable/disable for data print\n"
		   "Q: Quit\n"
		   );
}

void usage(const char* name)
{
	printf("Usage: %s [listening_port] [thread_count] [disable_data_print]\n", name);
}

void handle_ademco_msg(ThreadContext* context, bufferevent* bev)
{
	int fd = (int)bufferevent_getfd(bev);
	auto& client = context->clients[fd];
	if (!disable_data_print) {
		printf("T#%d C#%d acct=%s ademco_id=%06zX :%s\n",
			   context->worker_id, fd, client.acct.data(), client.ademco_id, context->packet.toString().data());
	}
	auto output = bufferevent_get_output(bev);
	switch (context->packet.id_.eid_) {
	case AdemcoId::id_null:
	case AdemcoId::id_hb:
	case AdemcoId::id_admcid:
		if (context->packet.id_.eid_ != AdemcoId::id_null) {
			client.acct = context->packet.acct_.acct();
			client.ademco_id = context->packet.ademcoData_.ademco_id_;
			if (ademco::isMachineTypeEvent(context->packet.ademcoData_.ademco_event_)) {
				client.type = context->packet.ademcoData_.ademco_event_;
			}
		}
		{
			char buf[1024];
			size_t n = context->packet.make_ack(buf, sizeof(buf), context->packet.seq_.value_, context->packet.acct_.acct(), context->packet.ademcoData_.ademco_id_);
			evbuffer_add(output, buf, n);
			if (!disable_data_print) {
				printf("T#%d S#%d acct=%s ademco_id=%06zX :%s\n",
					   context->worker_id, fd, client.acct.data(), client.ademco_id, context->packet.toString().data());
			}
		}
		break;
	default:
		break;
	}
}

// send all buffered events to all clients
void commandcb(evutil_socket_t, short, void* user_data)
{
	auto context = (ThreadContext*)user_data;
	std::vector<ADEMCO_EVENT> evs;

	{
		std::lock_guard<std::mutex> lg(mutex);
		if (--threads_to_handled_event == 0) {
			evs = std::move(events);
		} else {
			evs = events;
		}
	}

	char buf[1024];

	std::lock_guard<std::mutex> lg(context->mutex);
	for (auto& client : context->clients) {
		for (auto e : evs) {			
			size_t n = 0;
			if (e == EVENT_INVALID_EVENT) { // M
				n = context->packet.make_hb(buf, sizeof(buf), client.second.seq, client.second.acct, client.second.ademco_id, 
											userInput.gg, (ADEMCO_EVENT)userInput.ev, userInput.zone);
				evbuffer_add(client.second.output, buf, n);
				if (!disable_data_print) {
					printf("T#%d S#%d acct=%s ademco_id=%06zX :%s\n",
						   context->worker_id, client.second.fd, client.second.acct.data(), client.second.ademco_id, context->packet.toString().data());
				}
			} else if (e == (EVENT_INVALID_EVENT + 1)) { // C
				if (client.second.ademco_id != userInput.ademco_id) {
					continue;
				}
				n = context->packet.make_hb(buf, sizeof(buf), client.second.seq, client.second.acct, client.second.ademco_id,
											userInput.gg, (ADEMCO_EVENT)userInput.ev, userInput.zone);
				evbuffer_add(client.second.output, buf, n);
				if (!disable_data_print) {
					printf("T#%d S#%d acct=%s ademco_id=%06zX :%s\n",
						   context->worker_id, client.second.fd, client.second.acct.data(), client.second.ademco_id, context->packet.toString().data());
				}
			} else if (e == (EVENT_INVALID_EVENT + 2)) { // X
				if (client.second.ademco_id != userInput.ademco_id) {
					continue;
				}
				auto xdata = makeXData(userInput.xdata, strlen(userInput.xdata));
				n = context->packet.make_hb(buf, sizeof(buf), client.second.seq, client.second.acct, client.second.ademco_id,
											userInput.gg, (ADEMCO_EVENT)userInput.ev, userInput.zone, xdata);
				evbuffer_add(client.second.output, buf, n);
				if (!disable_data_print) {
					printf("T#%d S#%d acct=%s ademco_id=%06zX :%s\n",
						   context->worker_id, client.second.fd, client.second.acct.data(), client.second.ademco_id, context->packet.toString().data());
				}
			} else if (client.second.type == EVENT_I_AM_3_SECTION_MACHINE && (e == EVENT_ARM || e == EVENT_DISARM)) {
				for (int gg = 1; gg <= 3; gg++) {
					if (++client.second.seq == 10000) {
						client.second.seq = 1;
					}
					if (e == EVENT_DISARM) {
						auto xdata = makeXData(userInput.pwd, 6);
						n = context->packet.make_hb(buf, sizeof(buf), client.second.seq, client.second.acct, client.second.ademco_id, gg, e, 0, xdata);
					} else {
						n = context->packet.make_hb(buf, sizeof(buf), client.second.seq, client.second.acct, client.second.ademco_id, gg, e, 0);
					}
					evbuffer_add(client.second.output, buf, n);
					if (!disable_data_print) {
						printf("T#%d S#%d acct=%s ademco_id=%06zX :%s\n",
							   context->worker_id, client.second.fd, client.second.acct.data(), client.second.ademco_id, context->packet.toString().data());
					}
				}
			} else {
				if (++client.second.seq == 10000) {
					client.second.seq = 1;
				}
				if (e == EVENT_DISARM) {
					auto xdata = makeXData(userInput.pwd, 6);
					n = context->packet.make_hb(buf, sizeof(buf), client.second.seq, client.second.acct, client.second.ademco_id, 0, e, 0, xdata);
				} else {
					n = context->packet.make_hb(buf, sizeof(buf), client.second.seq, client.second.acct, client.second.ademco_id, 0, e, 0);
				}
				evbuffer_add(client.second.output, buf, n);
				if (!disable_data_print) {
					printf("T#%d S#%d acct=%s ademco_id=%06zX :%s\n",
						   context->worker_id, client.second.fd, client.second.acct.data(), client.second.ademco_id, context->packet.toString().data());
				}
			}
		}
	}
}

void readcb(struct bufferevent* bev, void* user_data)
{
	auto input = bufferevent_get_input(bev);
	auto context = (ThreadContext*)user_data;

	while (1) {
		size_t len = evbuffer_get_length(input);
		if (len < 9) { return; }
		char buff[1024] = { 0 };
		evbuffer_copyout(input, buff, std::min(len, sizeof(buff)));
		size_t cb_commited = 0;
		auto res = context->packet.parse(buff, 1024, cb_commited);
		bool done = false;
		switch (res) {
		case ademco::ParseResult::RESULT_OK:
			evbuffer_drain(input, cb_commited);
			handle_ademco_msg(context, bev);
			break;
		case ademco::ParseResult::RESULT_NOT_ENOUGH:
			done = true;
			break;
		case ademco::ParseResult::RESULT_DATA_ERROR:
			fprintf(stderr, "error while parsing data\n");
			evbuffer_drain(input, len);
			done = true;
			break;
		}

		if (done) { break; }
	}
}

void eventcb(struct bufferevent* bev, short events, void* user_data)
{
	int fd = (int)bufferevent_getfd(bev);
	printf("eventcb events=%04X\n", events);
	if (events & BEV_EVENT_EOF) {
	} else if (events & (BEV_EVENT_WRITING)) {
		printf("Got an error while writing #%d.\n", fd);
	} else if (events & (BEV_EVENT_ERROR)) {
		printf("Got an error on the connection %d: %s\n",
			   fd, strerror(errno));
	}
	printf("Connection #%d closed.\n", fd); 
	auto context = (ThreadContext*)user_data;
	{
		std::lock_guard<std::mutex> lg(context->mutex);
		context->clients.erase(fd);
	}
	bufferevent_free(bev);
}

void accept_cb(evconnlistener* listener, evutil_socket_t fd, sockaddr* addr, int socklen, void* user_data)
{
	char str[INET_ADDRSTRLEN] = { 0 };
	auto sin = (sockaddr_in*)addr;
	inet_ntop(AF_INET, &sin->sin_addr, str, INET_ADDRSTRLEN);
	printf("accpet TCP connection #%d from: %s:%d\n", (int)fd, str, sin->sin_port);
	evutil_make_socket_nonblocking(fd);

	static int worker_id = 0;
	auto context = worker_thread_contexts[worker_id];
	auto bev = bufferevent_socket_new(context->base, fd, BEV_OPT_CLOSE_ON_FREE);
	if (!bev) {
		fprintf(stderr, "Error constructing bufferevent!\n");
		event_base_loopbreak(context->base);
		return;
	}

	Client client;
	client.fd = (int)fd;
	client.output = bufferevent_get_output(bev);

	{
		std::lock_guard<std::mutex> lg(context->mutex);
		context->clients[(int)fd] = client;
	}

	bufferevent_setcb(bev, readcb, nullptr, eventcb, context);
	bufferevent_enable(bev, EV_WRITE | EV_READ);
	worker_id = (++worker_id) % thread_count;	
}

void accpet_error_cb(evconnlistener* listener, void* context)
{
	auto base = evconnlistener_get_base(listener);
	int err = EVUTIL_SOCKET_ERROR();
	fprintf(stderr, "accpet_error_cb:%d:%s\n", err, evutil_socket_error_to_string(err));
	event_base_loopexit(base, nullptr);
}

void init_listener_thread(const sockaddr_in& addr)
{
	listen_thread_evbase = event_base_new();
	if (!listen_thread_evbase) {
		fprintf(stderr, "init libevent failed\n");
		exit(-1);
	}

	auto listener = evconnlistener_new_bind(listen_thread_evbase,
											accept_cb,
											nullptr,
											LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,
											-1, // backlog, -1 for default
											(const sockaddr*)(&addr),
											sizeof(addr));
	if (!listener) {
		fprintf(stderr, "create listener failed\n");
		exit(-1);
	}
	evconnlistener_set_error_cb(listener, accpet_error_cb);
}

void dummy_cb_avoid_worker_exit(evutil_socket_t, short, void*)
{
}

ThreadContext* init_worker_thread(int i)
{
	ThreadContext* context = new ThreadContext();
	context->worker_id = i;
	context->base = event_base_new();
	if (!context->base) {
		fprintf(stderr, "init libevent failed\n");
		exit(-1);
	}
	timeval tv = { 1, 0 };
	event_add(event_new(context->base, -1, EV_PERSIST, dummy_cb_avoid_worker_exit, nullptr), &tv);
	printf("worker thread #%d created\n", i);
	return context;
}

int main(int argc, char** argv)
{
	usage(argv[0]);

#ifdef _WIN32
	WSADATA wsa_data;
	WSAStartup(0x0201, &wsa_data);
	if (0 != evthread_use_windows_threads()) {
		fprintf(stderr, "failed to init libevent with thread by calling evthread_use_windows_threads\n");
		return -1;
	}
	google_breakpad::ExceptionHandler eh(nullptr, // dump_path
										 nullptr, // FilterCallback 
										 dumpCallback, // MinidumpCallback 
										 nullptr, // callback_context
										 google_breakpad::ExceptionHandler::HANDLER_ALL // handler_types
	); // MINIDUMP_TYPE
#else 
	if (0 != evthread_use_pthreads()) {
		fprintf(stderr, "failed to init libevent with thread by calling evthread_use_pthreads\n");
		return -1;
	}
	google_breakpad::ExceptionHandler eh(google_breakpad::MinidumpDescriptor("./"), nullptr, dumpCallback, nullptr, true, -1);
#endif

	int port = 12345; 
	if (argc > 1) {
		port = atoi(argv[1]);
		if (port <= 0 || port > 65535) {
			puts("Invalid port");
			return 1;
		}
	}

	if (argc > 2) {
		thread_count = atoi(argv[2]);
		if (thread_count <= 0) {
			puts("Invalid thread_count");
			return 1;
		}
	}

	if (argc > 3) {
		disable_data_print = atoi(argv[3]) == 1;
	}

	sockaddr_in sin = { 0 };
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_port = htons(port);

	printf("using libevent %s\n", event_get_version());
	printf("%s is listening on port %d using %d threads, disable_data_print=%d\n",
		   argv[0], port, thread_count, disable_data_print);

	std::vector<std::thread> worker_threads;
	for (int i = 0; i < thread_count; i++) {
		auto context = init_worker_thread(i);
		worker_thread_contexts.push_back(context);
		worker_threads.emplace_back(std::thread([context]() {
			event_base_dispatch(context->base);
		}));
	}

	init_listener_thread(sin);
	std::thread listener_thread([]() { event_base_dispatch(listen_thread_evbase); });

	op_usage();

	auto fire_command = []() {
		struct timeval tv = { 0, 1000 };
		for (int i = 0; i < thread_count; i++) {
			event_add(event_new(worker_thread_contexts[i]->base, -1, 0, commandcb, worker_thread_contexts[i]), &tv);
		}
	};

	bool running = true;
	while (running) {
		int cmd = getchar();
		if ('a' <= cmd && cmd <= 'z') {
			cmd -= 32;
		}

		switch (cmd) {
		case 'A':
			{
				std::lock_guard<std::mutex> lg(mutex);
				events.push_back(EVENT_ARM);
				threads_to_handled_event = thread_count;
			}
			fire_command();
			break;

		case 'D':
			do {
				printf("Input 6 digit password:");
				scanf("%s", userInput.pwd);
			} while (strlen(userInput.pwd) != 6);
			{
				std::lock_guard<std::mutex> lg(mutex);
				events.push_back(EVENT_DISARM);
				threads_to_handled_event = thread_count;
			}
			fire_command();
			break;

		case 'E':
			{
				std::lock_guard<std::mutex> lg(mutex);
				events.push_back(EVENT_EMERGENCY);
				threads_to_handled_event = thread_count;
			}
			fire_command();
			break;

		case 'T':
			{
				std::lock_guard<std::mutex> lg(mutex);
				events.push_back(EVENT_WHAT_IS_YOUR_TYPE);
				threads_to_handled_event = thread_count;
			}
			fire_command();
			break;

		case 'M':
			{
				int ret = 0;
				do {
					printf("Input [event gg zone]:");
					ret = scanf("%d %d %d", &userInput.ev, &userInput.gg, &userInput.zone);
				} while (ret != 3);
				{
					std::lock_guard<std::mutex> lg(mutex);
					events.push_back(EVENT_INVALID_EVENT);
					threads_to_handled_event = thread_count;
				}
				fire_command();
				break;
			}

		case 'C':
			{
				int ret = 0;
				do {
					printf("Input [ademco_id event gg zone]:");
					ret = scanf("%d %d %d %d", &userInput.ademco_id, &userInput.ev, &userInput.gg, &userInput.zone);
				} while (ret != 4);
				{
					std::lock_guard<std::mutex> lg(mutex);
					events.push_back((ADEMCO_EVENT)(EVENT_INVALID_EVENT + 1));
					threads_to_handled_event = thread_count;
				}
				fire_command();
				break;
			}

		case 'X':
			{
				int ret = 0;
				do {
					printf("Input [ademco_id event gg zone xdata]:");
					ret = scanf("%d %d %d %d %s", &userInput.ademco_id, &userInput.ev, &userInput.gg, &userInput.zone, userInput.xdata);
				} while (ret != 5);
				{
					std::lock_guard<std::mutex> lg(mutex);
					events.push_back((ADEMCO_EVENT)(EVENT_INVALID_EVENT + 2));
					threads_to_handled_event = thread_count;
				}
				fire_command();
				break;
			}

		case 'I':
			{
				std::vector<Client> copiedClients;
				for (auto context : worker_thread_contexts) {
					std::lock_guard<std::mutex> lg(context->mutex);
					for (const auto& client : context->clients) {
						copiedClients.push_back(client.second);
					}
				}
				printf("Total connnected %zd clients:\n", copiedClients.size());
				for (const auto& client : copiedClients) {
					printf("  fd=#%d acct=%s ademco_id=%zd\n", client.fd, client.acct.data(), client.ademco_id);
				}
			}
			break;

		case 'P':
			disable_data_print = !disable_data_print;
			printf("Data print is %s\n", disable_data_print ? "Off" : "On");
			break;

		case 'Q':
			{
				timeval tv{ 0, 1000 };
				event_base_loopexit(listen_thread_evbase, &tv);
				listener_thread.join();
				printf("listener thread exited\n");
				for (int i = 0; i < thread_count; i++) {
					event_base_loopexit(worker_thread_contexts[i]->base, &tv);
					worker_threads[i].join();
					printf("worker_thread #%d exited\n", i);
				}
				running = false;
				break;
			}
			
		case '\r':
		case '\n':
			break;

		default:
			printf("Invalid command\n");
			op_usage();
			break;
		}		
	}

	printf("Bye\n");

	return 0;
}
