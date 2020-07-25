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
#include <thread>
#include <vector>
#include <mutex>
#include <string>

#include <event2/listener.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>

#if !defined(LIBEVENT_VERSION_NUMBER) || LIBEVENT_VERSION_NUMBER < 0x02010100
#error "This version of Libevent is not supported; Get 2.1.1-alpha or later."
#endif

#define DISABLE_JLIB_LOG2

#include <ademco_packet.h>

using namespace ademco;

void op_usage()
{
	printf("Press A for Arm, D for Disarm, E for Emergency, Q for Quit\n");
}

void usage(const char* name)
{
	printf("Usage: %s listening_port\n", name);
	op_usage();
}

struct Client {
	int fd = 0;
	std::string acct = {};
	int ademco_id = 0;
};

void readcb(struct bufferevent* bev, void* user_data)
{
	//evbuffer_add_buffer(bufferevent_get_output(bev), bufferevent_get_input(bev));
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
#ifdef _WIN32
	WSADATA wsa_data;
	WSAStartup(0x0201, &wsa_data);
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
	event_base_dispatch(base);

	return 0;
}
