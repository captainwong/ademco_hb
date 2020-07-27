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

#if !defined(LIBEVENT_VERSION_NUMBER) || LIBEVENT_VERSION_NUMBER < 0x02010100
#error "This version of Libevent is not supported; Get 2.1.1-alpha or later."
#endif

#define DISABLE_JLIB_LOG2
#include <ademco_packet.h>

using namespace ademco;

struct Client {
	int fd = 0;
	evbuffer* output = nullptr;
	std::string acct = {};
	int ademco_id = 0;
	int seq = 0;
};

std::unordered_map<int, Client> clients = {};

// events will be sent to all clients
std::vector<ADEMCO_EVENT> events = {};
std::mutex mutex = {};
char pwd[1024] = { 0 };

void op_usage()
{
	printf("Press A for Arm, D for Disarm, E for Emergency, Q for Quit\n");
}

void usage(const char* name)
{
	printf("Usage: %s listening_port\n", name);
}

void handle_ademco_msg(AdemcoPacket& packet, bufferevent* bev)
{
	int fd = bufferevent_getfd(bev);
	auto& client = clients[fd];
	printf("C#%d acct=%s ademco_id=%06X :%s\n", fd, client.acct.data(), client.ademco_id, packet.toString().data());
	auto output = bufferevent_get_output(bev);
	switch (packet.id_.eid_) {
	case AdemcoId::id_null:
	case AdemcoId::id_hb:
	case AdemcoId::id_admcid:
		if (packet.id_.eid_ != AdemcoId::id_null) {
			client.acct = packet.acct_.acct();
			client.ademco_id = packet.ademcoData_.ademco_id_;			
		}
		{
			char buf[1024];
			size_t n = packet.make_ack(buf, sizeof(buf), packet.seq_.value_, packet.acct_.acct(), packet.ademcoData_.ademco_id_);
			evbuffer_add(output, buf, n);
			printf("S#%d acct=%s ademco_id=%06X :%s\n", fd, client.acct.data(), client.ademco_id, packet.toString().data());
		}
		break;
	default:
		break;
	}
}

// send all buffered events to all clients
void commandcb(evutil_socket_t, short, void*)
{
	std::vector<ADEMCO_EVENT> evs;

	{
		std::lock_guard<std::mutex> lg(mutex);
		evs = std::move(events);
	}

	char buf[1024];
	AdemcoPacket maker;
	for (auto& client : clients) {
		for (auto e : evs) {
			size_t n = 0;
			if (e == EVENT_DISARM) {
				if (++client.second.seq == 10000) {
					client.second.seq = 1;
				}
				auto xdata = makeXData(pwd, 6);
				n = maker.make_hb(buf, sizeof(buf), client.second.seq, client.second.acct, client.second.ademco_id, 0, e, 0, xdata);
			} else {
				n = maker.make_hb(buf, sizeof(buf), client.second.seq, client.second.acct, client.second.ademco_id, 0, e, 0);
			}
			evbuffer_add(client.second.output, buf, n);
			printf("S#%d acct=%s ademco_id=%06X :%s\n", 
				   client.second.fd, client.second.acct.data(), client.second.ademco_id, maker.toString().data());
		}
	}
}

void readcb(struct bufferevent* bev, void* user_data)
{
	auto input = bufferevent_get_input(bev);

	while (1) {
		size_t len = evbuffer_get_length(input);
		if (len < 9) { return; }
		static AdemcoPacket parser = {};
		char buff[1024] = { 0 };
		evbuffer_copyout(input, buff, std::min(len, sizeof(buff)));
		size_t cb_commited = 0;
		auto res = parser.parse(buff, 1024, cb_commited);
		bool done = false;
		switch (res) {
		case ademco::ParseResult::RESULT_OK:
			evbuffer_drain(input, cb_commited);
			handle_ademco_msg(parser, bev);
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
	int fd = bufferevent_getfd(bev);
	printf("eventcb events=%04X\n", events);
	if (events & BEV_EVENT_EOF) {
	} else if (events & (BEV_EVENT_WRITING)) {
		printf("Got an error while writing #%d.\n", fd);
	} else if (events & (BEV_EVENT_ERROR)) {
		printf("Got an error on the connection %d: %s\n",
			   fd, strerror(errno));
	}
	printf("Connection #%d closed.\n", fd);
	clients.erase(fd);
	bufferevent_free(bev);
}

void accept_cb(evconnlistener* listener, evutil_socket_t fd, sockaddr* addr, int socklen, void* context)
{
	char str[INET_ADDRSTRLEN] = { 0 };
	auto sin = (sockaddr_in*)addr;
	inet_ntop(AF_INET, &sin->sin_addr, str, INET_ADDRSTRLEN);
	printf("accpet TCP connection from: %s:%d\n", str, sin->sin_port);

	//evutil_make_socket_nonblocking(fd);
	auto base = evconnlistener_get_base(listener);
	auto bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
	if (!bev) {
		fprintf(stderr, "Error constructing bufferevent!\n");
		event_base_loopbreak(base);
		return;
	}

	Client client;
	client.fd = fd;
	client.output = bufferevent_get_output(bev);
	clients[fd] = client;

	bufferevent_setcb(bev, readcb, nullptr, eventcb, nullptr);
	bufferevent_enable(bev, EV_WRITE | EV_READ);
}

void accpet_error_cb(evconnlistener* listener, void* context)
{
	auto base = evconnlistener_get_base(listener);
	int err = EVUTIL_SOCKET_ERROR();
	fprintf(stderr, "accpet_error_cb:%d:%s\n", err, evutil_socket_error_to_string(err));
	event_base_loopexit(base, nullptr);
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
#else 
	if (0 != evthread_use_pthreads()) {
		fprintf(stderr, "failed to init libevent with thread by calling evthread_use_pthreads\n");
		return -1;
	}
#endif

	int port = 12345; 
	if (argc > 1) {
		port = atoi(argv[1]);
		if (port <= 0 || port > 65535) {
			puts("Invalid port");
			return 1;
		}
	}

	sockaddr_in sin = { 0 };
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_port = htons(port);

	printf("using libevent %s\n", event_get_version());
	auto base = event_base_new();
	if (!base) {
		fprintf(stderr, "init libevent failed\n");
		return -1;
	}

	auto listener = evconnlistener_new_bind(base,
											accept_cb,
											nullptr,
											LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,
											-1, // backlog, -1 for default
											(sockaddr*)(&sin),
											sizeof(sin));
	if (!listener) {
		fprintf(stderr, "create listener failed\n");
		return -1;
	}
	printf("%s is listening on port %d\n", argv[0], port);
	evconnlistener_set_error_cb(listener, accpet_error_cb);

	std::thread t([&base]() {
		event_base_dispatch(base);
	});

	op_usage();

	while (1) {
		int cmd = getchar();
		struct timeval tv = { 0, 1000 };

		if (cmd == 'a' || cmd == 'A') {
			{
				std::lock_guard<std::mutex> lg(mutex);
				events.push_back(EVENT_ARM);
			}
			event_add(event_new(base, -1, 0, commandcb, nullptr), &tv);
		} else if (cmd == 'd' || cmd == 'D') {
			do {
				printf("Input 6 digit password:");
				scanf("%s", pwd);
			} while (strlen(pwd) != 6);
			{
				std::lock_guard<std::mutex> lg(mutex);
				events.push_back(EVENT_DISARM);
			}
			event_add(evtimer_new(base, commandcb, nullptr), &tv);
		} else if (cmd == 'e' || cmd == 'E') {
			{
				std::lock_guard<std::mutex> lg(mutex);
				events.push_back(EVENT_EMERGENCY);
			}
			event_add(evtimer_new(base, commandcb, nullptr), &tv);
		} else if (cmd == '\r' || cmd == '\n') {
		} else if (cmd == 'q' || cmd == 'Q') {
			event_base_loopbreak(base);
			t.join();
			break;
		} else {
			op_usage();
		}
	}

	printf("Bye\n");

	return 0;
}
