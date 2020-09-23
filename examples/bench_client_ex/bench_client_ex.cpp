#ifndef _WIN32
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/syscall.h>
#else
# ifndef  _CRT_SECURE_NO_WARNINGS
#  define  _CRT_SECURE_NO_WARNINGS
# endif
# ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#  define _WINSOCK_DEPRECATED_NO_WARNINGS
# endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#endif

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <time.h>
#include <string>
#include <algorithm>
#include <chrono>
#include <thread>
#include <mutex>
#include <unordered_set>
#include <condition_variable>
#include <unordered_map>

#include <event2/listener.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/thread.h>

#define DISABLE_JLIB_LOG2
#define ENABLE_COMMON_MACHINE_STATUS_TO_STRING
#define ENABLE_COMMON_MACHINE_TYPE_TO_STRING
#define ENABLE_COMMON_ZONE_PROPERTY_TO_STRING
#include <hb_helper.h>

using namespace ademco;
using namespace hb;
using namespace hb::common;

DECLARE_HB_COMMON_COM_CONSTEXPR_MEMBERS;

uint64_t gettid() {
#ifdef _WIN32
	return static_cast<uint64_t>(GetCurrentThreadId());
#else
	return static_cast<uint64_t>(::syscall(SYS_gettid));
#endif
}

int thread_count = 0;
std::string acct_base = {};
int session_count = 0;
int session_connected = 0;
size_t max_acct_len = 0;
int machine_type = 255;
int heartbeat_gap = 5;
int timeout = 0;
int print_data = 1;
std::mutex gmutex{};


// Commands
// M
#define COMMAND_M_EGZ EVENT_INVALID_EVENT
// C
#define COMMAND_C_AEGZ ((ADEMCO_EVENT)(EVENT_INVALID_EVENT + 1))
// X
#define COMMAND_X_AEGZX ((ADEMCO_EVENT)(EVENT_INVALID_EVENT + 2))
// Y
#define COMMAND_Y_AEGZX ((ADEMCO_EVENT)(EVENT_INVALID_EVENT + 3))
// Z
#define COMMAND_Z_QUERY_ZONE ((ADEMCO_EVENT)(EVENT_INVALID_EVENT + 4))
// F 
#define COMMAND_F_QUERY_TIMER ((ADEMCO_EVENT)(EVENT_INVALID_EVENT + 5))


// events will be sent to all clients
std::vector<ADEMCO_EVENT> commands = {};
int threads_to_handled_event = 0;
struct UserInput {
	char pwd[1024] = { 0 };
	AdemcoId ademco_id = 0;
	int ev = 0;
	AdemcoGG gg = 0;
	AdemcoZone zone = 0;
	char xdata[1024] = { 0 };
}userInput = {};

void op_usage()
{
	printf("Press a key and hit <enter>:\n"
		   "A: Arm\n"
		   "D: Disarm\n"
		   "E: Emergency\n"
		   "M: Mannualy input [event gg zone], Exampel Input: 'M' <enter> 3400 1 0 <enter>\n"
		   "C: Like M, not send to all clients, but send to specific client with ademco_id: [ademco_id event gg zone]\n"
		   "X: Like C, with xdata: [ademco_id event gg zone xdata]\n"
		   "Y: Like X, with xdata, but xdata is hex: [ademco_id event gg zone xdata], example: [1 1704 0 0 EBAB3FA176]\n"
		   "\n"
		   "I: Print clients info\n"
		   "P: Toggle enable/disable data print\n"
		   "Q: Quit\n"
	);
}

struct Session {
	int fd = 0;
	int thread_id = 0;
	bufferevent* bev = nullptr;
	event* life_timer = nullptr;
	event* heartbeat_timer = nullptr;
	int id = 0;
	std::string acct = {};
	AdemcoId ademco_id = 0;
	MachineType type = MachineType::Gprs;
	MachineStatus status = MachineStatus::Arm;
	MachineStatus status1 = MachineStatus::Arm;
	MachineStatus status2 = MachineStatus::Arm;
	MachineStatus status3 = MachineStatus::Arm;
	uint16_t seq = 0;
	AdemcoPacket packet = {};

	uint16_t nextSeq() { if (++seq == 10000) { seq = 1; } return seq; }
	void printInfo() const;
};

struct ThreadContext {
	int thread_id = 0;
	event_base* base = nullptr;
	sockaddr_in addr = {};
	int session_start = 0;
	int session_end = 0;
	std::unordered_map<int, Session*> clients{};
	std::mutex mutex{};

	void connectNext();
};
typedef ThreadContext* ThreadContextPtr;

std::vector<ThreadContextPtr> threadContexts{};

void handle_ademco_msg(Session* session, bufferevent* bev)
{
	if (print_data) {
		printf("T#%d C#%d recv:%s\n",session->thread_id, session->id, session->packet.toString().data());
	}
	char buf[1024];
	auto output = bufferevent_get_output(bev);
	switch (session->packet.id_.eid_) {
	case AdemcoMsgId::id_ack:
		/*if (load_test) {
			auto now = std::chrono::steady_clock::now();
			char buf[1024];
			session->lastTimePacketSize = session->packet.make_null(buf, sizeof(buf), session->nextSeq(), session->acct, session->ademco_id);
			evbuffer_add(output, buf, session->lastTimePacketSize);
			auto us = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - now).count();
			session->encodeTime += us;
		}*/
		break;

	case AdemcoMsgId::id_hb:
	case AdemcoMsgId::id_admcid:
		{
			switch (session->packet.ademcoData_.ademco_event_) {
			case EVENT_WHAT_IS_YOUR_TYPE:
				{
					auto n = session->packet.make_hb(buf, sizeof(buf),
													 session->packet.seq_.value_, session->acct, session->ademco_id, 0,
													 ademcoEventFromMachineType(session->type), 0);
					if (print_data) {
						printf("T#%d C#%d send:%s\n", session->thread_id, session->id, session->packet.toString().data());
					}
					evbuffer_add(output, buf, n);
					break;
				}

			case EVENT_ARM:
				{
					if (session->type == MachineType::ThreeSection && 1 <= session->packet.ademcoData_.gg_ && session->packet.ademcoData_.gg_ <= 3) {

					}
				}
			}
		}

	default:
		break;
	}
}

void readcb(struct bufferevent* bev, void* user_data)
{
	auto input = bufferevent_get_input(bev);
	auto session = (Session*)user_data;

	while (1) {
		size_t len = evbuffer_get_length(input);
		if (len < 9) { return; }
		char buff[1024] = { 0 };
		len = std::min(len, sizeof(buff));
		evbuffer_copyout(input, buff, len);
		size_t cb_commited = 0;
		auto res = session->packet.parse(buff, len, cb_commited);
		bool done = false;
		switch (res) {
		case ademco::ParseResult::RESULT_OK:
			evbuffer_drain(input, cb_commited);
			handle_ademco_msg(session, bev);
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

void writecb(struct bufferevent* bev, void* user_data)
{
	auto session = (Session*)user_data;
	
}

void heartbeat_timer_cb(evutil_socket_t fd, short what, void* user_data)
{
	auto session = (Session*)user_data;
	session->heartbeat_timer = event_new(threadContexts[session->thread_id]->base, -1, 0, heartbeat_timer_cb, session);
	if (!session->heartbeat_timer) {
		fprintf(stderr, "create heartbeat_timer failed\n");
		event_base_loopbreak(threadContexts[session->thread_id]->base);
		return;
	}
	struct timeval heartbeat_tv = { heartbeat_gap, 0 };
	event_add(session->heartbeat_timer, &heartbeat_tv);

	char buf[1024];
	auto n = session->packet.make_null(buf, sizeof(buf), session->nextSeq(), session->acct, session->ademco_id);
	evbuffer_add(bufferevent_get_output(session->bev), buf, n);

	if (print_data) {
		printf("T#%d C#%d send:%s\n", session->thread_id, session->id, session->packet.toString().data());
	}
}

void timer_cb(evutil_socket_t, short, void* arg)
{
	auto session = (Session*)arg;
	printf("T#%d C#%d timeout\n", session->thread_id, session->id);
	session->life_timer = nullptr;
	if (session->heartbeat_timer) {
		event_del(session->heartbeat_timer);
		session->heartbeat_timer = nullptr;
	}
	bufferevent_disable(session->bev, EV_WRITE);
	// SHUT_WR
	shutdown(session->fd, 1);
}

void eventcb(struct bufferevent* bev, short events, void* user_data)
{
	auto session = (Session*)user_data;
	printf("eventcb on session #%d events=%04X\n", session->id, events);
	auto ctx = threadContexts[session->thread_id];
	if (events & BEV_EVENT_CONNECTED) {
		{
			std::lock_guard<std::mutex> lg(ctx->mutex);
			ctx->clients[session->ademco_id] = session;
		}
		printf("T#%d C#%d ademco_id=%06X connected, fd=%d\n", session->thread_id, session->id, session->ademco_id, session->fd);
		{
			std::lock_guard<std::mutex> lg(gmutex);
			printf("live connections %d\n", ++session_connected);
			if (session_connected == session_count) {
				printf("All connected\n");
			}
		}
		char buf[1024];
		auto n = session->packet.make_hb(buf, sizeof(buf),
										 session->nextSeq(), session->acct, session->ademco_id, 0, 
										 ademcoEventFromMachineStatus(session->status), 0);
		if (print_data) {
			printf("T#%d C#%d send:%s\n", session->thread_id, session->id, session->packet.toString().data());
		}
		n += session->packet.make_hb(buf + n, sizeof(buf) - n,
									 session->nextSeq(), session->acct, session->ademco_id, 0, 
									 ademcoEventFromMachineType(session->type), 0);
		if (print_data) {
			printf("T#%d C#%d send:%s\n", session->thread_id, session->id, session->packet.toString().data());
		}
		evbuffer_add(bufferevent_get_output(bev), buf, n);

		auto base = bufferevent_get_base(bev);
		session->life_timer = event_new(base, -1, EV_TIMEOUT, timer_cb, session);
		if (!session->life_timer) {
			fprintf(stderr, "create life_timer failed\n");
			event_base_loopbreak(base);
			return;
		}
		struct timeval tv = { timeout, 0 };
		event_add(session->life_timer, &tv);

		session->heartbeat_timer = event_new(base, -1, EV_TIMEOUT, heartbeat_timer_cb, session);
		if (!session->heartbeat_timer) {
			fprintf(stderr, "create heartbeat_timer failed\n");
			event_base_loopbreak(base);
			return;
		}
		tv.tv_sec = heartbeat_gap;
		event_add(session->heartbeat_timer, &tv);

		

		if (++ctx->session_start < ctx->session_end) {
			ctx->connectNext();
		}

		return;
	} else if (events & (BEV_EVENT_EOF)) {
		printf("T#%d C#%d fd=%d acct=%s ademco_id=%06X closed.\n", session->thread_id, session->id, session->fd, session->acct.data(), session->ademco_id);
	} else if (events & BEV_EVENT_ERROR) {
		printf("T#%d C#%d ERROR: %s\n",
			   session->thread_id, session->id, strerror(errno));
	}

	{
		std::lock_guard<std::mutex> lg(ctx->mutex);
		ctx->clients.erase(session->ademco_id);
	}

	{
		std::lock_guard<std::mutex> lg(gmutex);
		--session_connected;
		printf("live connections %d\n", session_connected);
		if (session_connected == 0) {
			printf("All disconnected\n");			
		}
	}

	if (session->heartbeat_timer) {
		event_del(session->heartbeat_timer);
		session->heartbeat_timer = nullptr;
	}
	if (session->life_timer) {
		event_del(session->life_timer);
		session->life_timer = nullptr;
	}
	delete session;
	bufferevent_free(bev);
	//event_base_loopbreak(bufferevent_get_base(bev));	
}

ThreadContext* init_thread(int thread_id, const sockaddr_in& sin, int session_start, int session_per_thread)
{
	printf("init_thread T#%d session_start=%d, session_end=%d\n", thread_id, session_start, session_start + session_per_thread);
	ThreadContext* context = new ThreadContext();
	context->thread_id = thread_id;
	context->base = event_base_new();
	if (!context->base) {
		fprintf(stderr, "init libevent failed\n");
		exit(-1);
	}
	memcpy(&context->addr, &sin, sizeof(sin));
	context->session_start = session_start;
	context->session_end = session_start + session_per_thread;
	threadContexts[thread_id] = context;
	srand(session_start);
	context->connectNext();

	return context;
}

void ThreadContext::connectNext()
{
	auto bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);
	if (!bev) {
		fprintf(stderr, "allocate bufferevent failed\n");
		exit(-1);
	}
	auto session = new Session();
	session->thread_id = thread_id;
	session->bev = bev;
	session->id = session_start;
	session->ademco_id = session_start + 1;
	session->seq = 0;
	if (machine_type == -1) {
		do {
			session->type = (MachineType)(rand() % MachineType::MachineTypeCount);
		} while (!machineIsSelling(session->type));
	} else {
		session->type = (MachineType)machine_type;
	}
	if (session_count == 1) {
		session->acct = acct_base;
	} else {
		std::string acct_session = std::to_string(session_start);
		size_t acct_session_len = max_acct_len - acct_base.size();
		while (acct_session.size() < acct_session_len) {
			acct_session.insert(acct_session.begin(), '0');
		}
		session->acct = acct_base + acct_session;
	}
	bufferevent_setcb(bev, readcb, writecb, eventcb, session);
	printf("T#%d C#%d ademco_id=%06X connecting...\n", thread_id, session->id, session->ademco_id);
	bufferevent_enable(bev, EV_READ | EV_WRITE);
	if (bufferevent_socket_connect(bev, (const sockaddr*)(&addr), sizeof(addr)) < 0) {
		fprintf(stderr, "error starting connection\n");
		exit(-1);
	}
	session->fd = (int)bufferevent_getfd(bev);
}

void usage(const char* argv0)
{
	printf("Simulate alarm machines to connect to direct server, while connected, machines will start send heartbeats, user can also input commands to test."
		   "After timeout passed, simulation will stop and show stats\n"
		   "Usage: %s ip port thread_count session_count acct_base machine_type heartbeat_gap timeout [print_data]\n"
		   "  ip           : transmit server' ip\n"
		   "  port         : transmit server's port\n"
		   "  thread_count : threads to use\n"
		   "  session_count: alarm machines to simulate\n"
		   "  acct_base    : simulated alarm machines account base, their accounts will be in acct_base + to_string(session_id) form\n", argv0);
	printf("  machine_type : -1 for random type, valid types are:\n");
	for (auto type : AllMachineTypes) {
		if (machineIsSelling(type))
			printf("                 %s\n", machineTypeToString(type));
	}
	printf("  heartbeat_gap: secends between two heatbeats\n"
		   "  timeout      : close clients after seconds\n"
		   "  print_data   : print sent/recved data\n");
}

void clear_stdin()
{
	int ret = scanf("%*[^\n]%*c");
	(void)ret;
}

// send all buffered events to all clients
void commandcb(evutil_socket_t, short, void* user_data)
{
	auto context = (ThreadContext*)user_data;
	std::vector<ADEMCO_EVENT> evs;

	{
		std::lock_guard<std::mutex> lg(gmutex);
		if (--threads_to_handled_event == 0) {
			evs = std::move(commands);
		} else {
			evs = commands;
		}
	}

	char buf[1024];

	std::lock_guard<std::mutex> lg(context->mutex);
	for (auto iter : context->clients) {
		auto client = iter.second;
		auto output = bufferevent_get_output(client->bev);
		for (auto e : evs) {
			size_t n = 0;
			//if (e == COMMAND_M_EGZ) { // M
			//	n = client->packet.make_hb(buf, sizeof(buf), client->nextSeq(), client->acct, client->ademco_id,
			//								userInput.gg, (ADEMCO_EVENT)userInput.ev, userInput.zone);
			//	evbuffer_add(client.second.output, buf, n);
			//	if (!disable_data_print) {
			//		printf("T#%d S#%d acct=%s ademco_id=%06X :%s\n",
			//			   context->worker_id, client.second.fd, client.second.acct.data(), client.second.ademco_id,
			//			   context->packet.toString().data());
			//	}
			//} else if (e == (COMMAND_C_AEGZ)) { // C
			//	if (client.second.ademco_id != userInput.ademco_id) {
			//		continue;
			//	}
			//	n = context->packet.make_hb(buf, sizeof(buf), client.second.nextSeq(), client.second.acct, client.second.ademco_id,
			//								userInput.gg, (ADEMCO_EVENT)userInput.ev, userInput.zone);
			//	evbuffer_add(client.second.output, buf, n);
			//	if (!disable_data_print) {
			//		printf("T#%d S#%d acct=%s ademco_id=%06X :%s\n",
			//			   context->worker_id, client.second.fd, client.second.acct.data(), client.second.ademco_id,
			//			   context->packet.toString().data());
			//	}
			//} else if (e == (COMMAND_X_AEGZX)) { // X
			//	if (client.second.ademco_id != userInput.ademco_id) {
			//		continue;
			//	}
			//	auto xdata = makeXData(userInput.xdata, strlen(userInput.xdata));
			//	n = context->packet.make_hb(buf, sizeof(buf), client.second.nextSeq(), client.second.acct, client.second.ademco_id,
			//								userInput.gg, (ADEMCO_EVENT)userInput.ev, userInput.zone, xdata);
			//	evbuffer_add(client.second.output, buf, n);
			//	if (!disable_data_print) {
			//		printf("T#%d S#%d acct=%s ademco_id=%06X :%s\n",
			//			   context->worker_id, client.second.fd, client.second.acct.data(), client.second.ademco_id,
			//			   context->packet.toString().data());
			//	}
			//} else if (e == (COMMAND_Y_AEGZX)) { // Y
			//	if (client.second.ademco_id != userInput.ademco_id) {
			//		continue;
			//	}
			//	std::vector<char> data(strlen(userInput.xdata) / 2);
			//	ademco::detail::ConvertHiLoAsciiToHexCharArray(&data[0], userInput.xdata, strlen(userInput.xdata));
			//	auto xdata = makeXData(data);
			//	n = context->packet.make_hb(buf, sizeof(buf), client.second.nextSeq(), client.second.acct, client.second.ademco_id,
			//								userInput.gg, (ADEMCO_EVENT)userInput.ev, userInput.zone, xdata);
			//	evbuffer_add(client.second.output, buf, n);
			//	if (!disable_data_print) {
			//		printf("T#%d S#%d acct=%s ademco_id=%06X :%s\n",
			//			   context->worker_id, client.second.fd, client.second.acct.data(), client.second.ademco_id,
			//			   context->packet.toString(ademco::detail::ToStringOption::ALL_CHAR_AS_HEX).data());
			//	}
			//} else if (e == COMMAND_Z_QUERY_ZONE) {
			//	if (client.second.ademco_id != userInput.ademco_id) {
			//		continue;
			//	}
			//	n = context->packet.make_hb(buf, sizeof(buf), client.second.nextSeq(), client.second.acct, client.second.ademco_id, 0,
			//								EVENT_ENTER_SET_MODE, 0);
			//	evbuffer_add(client.second.output, buf, n);
			//	client.second.zones.clear();
			//	client.second.queryStage = QueryStage::WaitingSettingsMode;
			//	if (!disable_data_print) {
			//		printf("T#%d S#%d acct=%s ademco_id=%06X :%s\n",
			//			   context->worker_id, client.second.fd, client.second.acct.data(), client.second.ademco_id,
			//			   context->packet.toString(ademco::detail::ToStringOption::ALL_CHAR_AS_HEX).data());
			//	}

			//} else if (e == COMMAND_F_QUERY_TIMER) { // F
			//	if (client.second.ademco_id != userInput.ademco_id) {
			//		continue;
			//	}
			//	n = context->packet.make_hb(buf, sizeof(buf), client.second.nextSeq(), client.second.acct, client.second.ademco_id, 0,
			//								EVENT_ENTER_SET_MODE, 0);
			//	evbuffer_add(client.second.output, buf, n);
			//	client.second.zones.clear();
			//	client.second.queryStage = QueryStage::WaitingSettingsMode2;
			//	if (!disable_data_print) {
			//		printf("T#%d S#%d acct=%s ademco_id=%06X :%s\n",
			//			   context->worker_id, client.second.fd, client.second.acct.data(), client.second.ademco_id,
			//			   context->packet.toString(ademco::detail::ToStringOption::ALL_CHAR_AS_HEX).data());
			//	}
			//} else 
			if (client->type == EVENT_I_AM_3_SECTION_MACHINE && (e == EVENT_ARM || e == EVENT_DISARM)) {
				for (AdemcoGG gg = 1; gg <= 3; gg++) {
					n = client->packet.make_hb(buf, sizeof(buf), client->nextSeq(), client->acct, client->ademco_id, gg, e, 0);
					evbuffer_add(output, buf, n);
					if (print_data) {
						printf("T#%d C#%d send:%s\n", context->thread_id, client->id,  client->packet.toString().data());
					}
				}
			} else {
				n = client->packet.make_hb(buf, sizeof(buf), client->nextSeq(), client->acct, client->ademco_id, 0, e, 0);
				evbuffer_add(output, buf, n);
				if (print_data) {
					printf("T#%d C#%d send:%s\n", context->thread_id, client->id, client->packet.toString().data());
				}
			}
		}
	}
}
int main(int argc, char** argv)
{
#ifdef _WIN32
	WSADATA wsa_data;
	WSAStartup(0x0201, &wsa_data);
	if (0 != evthread_use_windows_threads()) {
		fprintf(stderr, "failed to init libevent with thread by calling evthread_use_windows_threads\n");
		return -1;
	}
#else 
	if (0 != evthread_use_pthreads()) {
		fprintf(stderr, "failed to init libevent with thread by calling evthread_use_pthreads\n");
		return -1;
	}
#endif

	if (argc < 9) {
		usage(argv[0]);
		return 1;
	}

	auto ip = argv[1];
	int port = atoi(argv[2]);
	if (port <= 0 || port > 65535) {
		puts("Invalid port");
		usage(argv[0]);
		return 1;
	}
	thread_count = atoi(argv[3]);
	if (thread_count <= 0) {
		puts("Invalid thread_count");
		usage(argv[0]);
		return 1;
	}
	session_count = atoi(argv[4]);
	if (session_count <= 0) {
		puts("Invalid session_count");
		usage(argv[0]);
		return 1;
	}
	if (thread_count > session_count) {
		puts("thread_count must not bigger than session_count");
		usage(argv[0]);
		return 1;
	}
	if (session_count % thread_count) {
		puts("session_count must times thread_count");
		usage(argv[0]);
		return 1;
	}
	acct_base = argv[5];
	if (acct_base.size() > 18) {
		puts("acct_base is too long, max acct length is 18");
		usage(argv[0]);
		return 1;
	}
	if (session_count == 1) {
		max_acct_len = acct_base.size();
	} else {
		max_acct_len = acct_base.size() + std::to_string(session_count - 1).size();
	}
	if (18 < max_acct_len) {
		puts("acct_base is too long or session_count is too large/invalid, max acct length is 18");
		usage(argv[0]);
		return 1;
	}
	machine_type = atoi(argv[6]);
	if (machine_type != -1 && !machineIsSelling(MachineType(machine_type))) {
		puts("Invalid machine type");
		usage(argv[0]);
		return 1;
	} 
	heartbeat_gap = atoi(argv[7]);
	if (heartbeat_gap <= 0) {
		puts("Invalid heartbeat_gap");
		usage(argv[0]);
		return 1;
	}
	timeout = atoi(argv[8]);
	if (timeout <= 0) {
		puts("Invalid timeout");
		usage(argv[0]);
		return 1;
	}
	if (argc > 9) {
		print_data = atoi(argv[9]) == 1;
	}

	printf("using libevent %s\n", event_get_version());
	size_t session_per_thread = session_count / thread_count;
	std::string acct_session_min = acct_base;
	while (acct_session_min.size() < max_acct_len) {
		acct_session_min.push_back('0');
	}

	std::string acct_session_max;
	if (session_count == 1) {
		acct_session_max = acct_session_min;
	} else {
		acct_session_max = std::to_string(session_count - 1);
		size_t acct_session_len = max_acct_len - acct_base.size();
		while (acct_session_max.size() < acct_session_len) {
			acct_session_max.insert(acct_session_max.begin(), '0');
		}
		acct_session_max = acct_base + acct_session_max;
	}
	printf("starting %s to %s:%d with %d threads, %d sessions, machine_type is %s, %zd sessions per thread, heartbeat_gap=%ds, timeout=%ds\n"
		   "acct_base=%s, acct range is %s to %s\n",
		   argv[0], ip, port, thread_count, session_count,
		   machine_type == -1 ? "random type" : machineTypeToString((MachineType)machine_type),
		   session_per_thread, heartbeat_gap, timeout,
		   acct_base.data(), acct_session_min.data(), acct_session_max.data());

	sockaddr_in sin = { 0 };
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr(ip);
	sin.sin_port = htons(port);

	std::vector<std::thread> threads;
	//threadContexts = new ThreadContextPtr[thread_count];

	threadContexts.resize(thread_count);
	for (int i = 0; i < thread_count; i++) {
		threads.emplace_back(std::thread([&sin, i, session_per_thread]() {
			auto context = init_thread(i, sin, i * (int)session_per_thread, (int)session_per_thread);
			event_base_dispatch(context->base);
		}));
	}

	auto fire_command = []() {
		struct timeval tv = { 0, 1000 };
		for (int i = 0; i < thread_count; i++) {
			event_add(event_new(threadContexts[i]->base, -1, 0, commandcb, threadContexts[i]), &tv);
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
				std::lock_guard<std::mutex> lg(gmutex);
				commands.push_back(EVENT_ARM);
				threads_to_handled_event = thread_count;
			}
			fire_command();
			break;

		case 'D':
			{
				std::lock_guard<std::mutex> lg(gmutex);
				commands.push_back(EVENT_DISARM);
				threads_to_handled_event = thread_count;
			}
			fire_command();
			break;

		case 'E':
			{
				std::lock_guard<std::mutex> lg(gmutex);
				commands.push_back(EVENT_EMERGENCY);
				threads_to_handled_event = thread_count;
			}
			fire_command();
			break;

		case 'I':
			{
				std::vector<Session> copiedClients;
				for (int i = 0; i < thread_count; i++) {
					auto context = threadContexts[i];
					std::lock_guard<std::mutex> lg(context->mutex);
					for (const auto& client : context->clients) {
						copiedClients.push_back(*client.second);
					}
				}
				printf("Total connnected %zd clients:\n", copiedClients.size());

				for (const auto& client : copiedClients) {
					client.printInfo();
				}
			}
			break;

		case 'P':
			print_data = !print_data;
			printf("Data print is %s\n", print_data ? "Off" : "On");
			break;

		case 'Q':
			{
				timeval tv{ 0, 1000 };
				for (int i = 0; i < thread_count; i++) {
					event_base_loopexit(threadContexts[i]->base, &tv);
					threads[i].join();
					printf("worker_thread #%d exited\n", i);
				}
				running = false;
				break;
			}

		case '\r':
		case '\n':
			break;

		default:
			clear_stdin();
			printf("Invalid command\n");
			op_usage();
			break;
		}
	}

	printf("Bye\n");

	return 0;
}

void Session::printInfo() const
{
	if (type == MachineType::ThreeSection) {
		printf("  T#%d C#%d fd=#%d acct=%s ademco_id=%d type=%s\n",
			   thread_id, id, fd, acct.data(), ademco_id,
			   machineTypeToString(type));
		printf("    status1: %d %s    status2: %d %s    status3: %d %s\n",
			   (int)status1, machineStatusToString(status1),
			   (int)status2, machineStatusToString(status2),
			   (int)status3, machineStatusToString(status3));
	} else {
		printf("  T#%d C#%d fd=#%d acct=%s ademco_id=%d type=%s status=%d %s\n",
			   thread_id, id, fd, acct.data(), ademco_id,
			   machineTypeToString(type),
			   (int)status, machineStatusToString(status));
	}
	/*for (const auto& zp : zones) {
		char buf[512];
		snprintf(buf, sizeof(buf), getZoneFormatString(machineTypeFromAdemcoEvent((ADEMCO_EVENT)type)), zp.first);
		printf("    Zone:");
		printf("%s", buf);
		printf("  Prop:0x%02X %s     \tLost Report Enabled:%s\n",
			   zp.second.prop, zonePropertyToStringEn(zp.second.prop),
			   zp.second.lost_report_enabled ? "true" : "false");
	}
	for (int i = 0; i < 2; i++) {
		if (com::isValidMachineTimer(timer.timer[i])) {
			printf("    Machine Timer#%d: Arm at %02d:%02d, Disarm at %02d:%02d\n", i + 1,
				   timer.timer[i].armAt.hour, timer.timer[i].armAt.minute,
				   timer.timer[i].disarmAt.hour, timer.timer[i].disarmAt.minute);
		} else {
			printf("    Machine Timer#%d: Not Set\n", i + 1);
		}
	}*/
}
