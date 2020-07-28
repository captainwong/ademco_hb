/**
Ping Pong Client
*/

#ifndef _WIN32
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#else
# ifndef  _CRT_SECURE_NO_WARNINGS
#  define  _CRT_SECURE_NO_WARNINGS
# endif
# ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#  define _WINSOCK_DEPRECATED_NO_WARNINGS
# endif
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#endif

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <string>
#include <algorithm>
#include <chrono>
#include <thread>
#include <mutex>

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

int thread_count = 0;
int session_count = 0;
int session_connected = 0;
int timeout = 0;

std::mutex mutex = {};
int64_t totalBytesRead = 0;
int64_t totalBytesWrite = 0;
int64_t totalMsgRead = 0;
int64_t totalMsgWritten = 0;
int64_t totalEncodeTime = 0;
int64_t totalDecodeTime = 0;

struct Session {
	int id = 0;
	std::string acct = {};
	size_t ademco_id = 0;
	uint16_t seq = 0;

	AdemcoPacket packet = {};
	size_t lastTimePacketSize = 0;

	int64_t bytesRead = 0;
	int64_t bytesWritten = 0;
	int64_t msgRead = 0;
	int64_t msgWritten = 0;
	int64_t encodeTime = 0;
	int64_t decodeTime = 0;
};


void handle_ademco_msg(Session* session, bufferevent* bev)
{
	auto output = bufferevent_get_output(bev);
	switch (session->packet.id_.eid_) {
	case AdemcoId::id_ack:
		if (session->packet.seq_.value_ == session->seq) {
			if (++session->seq == 10000) {
				session->seq = 1;
			}
		}
		{
			auto now = std::chrono::steady_clock::now();
			char buf[1024];
			session->lastTimePacketSize = session->packet.make_null(buf, sizeof(buf), session->seq, session->acct, session->ademco_id);
			evbuffer_add(output, buf, session->lastTimePacketSize);
			auto us = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - now).count();
			session->encodeTime += us;
		}
		break;
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
		evbuffer_copyout(input, buff, std::min(len, sizeof(buff)));
		size_t cb_commited = 0;
		auto now = std::chrono::steady_clock::now();
		auto res = session->packet.parse(buff, 1024, cb_commited);
		auto us = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - now).count();
		session->decodeTime += us;
		bool done = false;
		switch (res) {
		case ademco::ParseResult::RESULT_OK:
			evbuffer_drain(input, cb_commited);
			session->bytesRead += cb_commited;
			session->msgRead++;
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
	if (0 == evbuffer_get_length(bufferevent_get_output(bev))) {
		session->bytesWritten += session->lastTimePacketSize;
		session->msgWritten++;
	}

}

void timer_cb(evutil_socket_t fd, short what, void* arg)
{
	auto bev = (bufferevent*)arg;
	Session* session = nullptr;
	bufferevent_getcb(bev, nullptr, nullptr, nullptr, (void**)&session);
	printf("client #%d timeout\n", session->id);

	//auto bev = (bufferevent*)arg;
	//bufferevent_free(bev);
	//evutil_closesocket(fd);

	bufferevent_disable(bev, EV_WRITE);
	// SHUT_WR
	shutdown(fd, 1);
}

void eventcb(struct bufferevent* bev, short events, void* user_data)
{
	auto session = (Session*)user_data;
	printf("eventcb #%d events=%04X\n", session->id, events);
	if (events & BEV_EVENT_CONNECTED) {
		printf("client #%d connected\n", session->id);
		{
			std::lock_guard<std::mutex> lg(mutex);
			if (++session_connected == session_count) {
				printf("All connected\n");
			}
		}
		char buf[1024];
		session->lastTimePacketSize = session->packet.make_null(buf, sizeof(buf), session->seq, session->acct, session->ademco_id);
		evbuffer_add(bufferevent_get_output(bev), buf, session->lastTimePacketSize);
		auto base = bufferevent_get_base(bev);
		auto timer = event_new(base, bufferevent_getfd(bev), EV_TIMEOUT, timer_cb, bev);
		if (!timer) {
			fprintf(stderr, "create timer failed\n");
			event_base_loopbreak(base);
			return;
		}
		struct timeval tv = { timeout, 0 };
		event_add(timer, &tv);
		return;
	} else if (events & (BEV_EVENT_EOF)) {
		printf("Connection #%d closed.\n", session->id);
	} else if (events & BEV_EVENT_ERROR) {
		printf("Got an error on the connection #%d: %s\n",
			   session->id, strerror(errno));
	}

	{
		std::lock_guard<std::mutex> lg(mutex);
		totalBytesRead += session->bytesRead;
		totalBytesWrite += session->bytesWritten;
		totalMsgRead += session->msgRead;
		totalMsgWritten += session->msgWritten;
		totalEncodeTime += session->encodeTime;
		totalDecodeTime += session->decodeTime;

		if (--session_connected == 0) {
			printf("All disconnected\n");
			double encodeSpeed = totalEncodeTime * 1.0 / totalMsgWritten;
			double decodeSpeed = totalDecodeTime * 1.0 / totalMsgRead;
			printf("Read %.2f MiB %lld packets, Write %.2f MiB %lld packets\n"
				   "Average msg size is %.2f bytes\n"
				   "Throughput is %.2f MiB/s %.2f packets/s\n"
				   "Encode average time %.2f us, %.2f packets/s\n"
				   "Decode average time %.2f us, %.2f packets/s\n",
				   totalBytesRead / 1024.0 / 1024, totalMsgRead, totalBytesWrite / 1024.0 / 1024, totalMsgWritten,
				   (totalBytesRead + totalBytesWrite) * 1.0 / (totalMsgRead + totalMsgWritten),
				   totalBytesRead * 1.0 / (timeout * 1024.0 * 1024), totalMsgRead * 1.0 / timeout,
				   encodeSpeed, 1000000.0 / encodeSpeed,
				   decodeSpeed, 1000000.0 / decodeSpeed);
		}
	}
	
	delete session;
	bufferevent_free(bev);
}

event_base* init_thread(const sockaddr_in& sin, int session_start, int session_per_thread)
{
	auto base = event_base_new();
	if (!base) {
		fprintf(stderr, "init libevent failed\n");
		exit(-1);
	}

	for (int i = 0; i < session_per_thread; i++) {
		auto bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);
		if (!bev) {
			fprintf(stderr, "allocate bufferevent failed\n");
			exit(-1);
		}
		auto session = new Session();
		session->id = i + session_start;
		session->acct = std::string("861234567890") + std::to_string(i + session_start);
		session->ademco_id = i + session_start;
		session->seq = 1;
		bufferevent_setcb(bev, readcb, writecb, eventcb, session);
		bufferevent_enable(bev, EV_READ | EV_WRITE);

		if (bufferevent_socket_connect(bev, (const sockaddr*)(&sin), sizeof(sin)) < 0) {
			fprintf(stderr, "error starting connection\n");
			exit(-1);
		}
	}

	return base;
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

	if (argc < 6) {
		printf("Usage: %s ip port thread_count session_count timeout\ntimeout is in seconds\n", argv[0]);
		return 1;
	}

	auto ip = argv[1];
	int port = atoi(argv[2]);
	if (port <= 0 || port > 65535) {
		puts("Invalid port");
		return 1;
	}
	thread_count = atoi(argv[3]);
	if (thread_count <= 0) {
		puts("Invalid thread_count");
		return 1;
	}
	session_count = atoi(argv[4]);
	if (session_count <= 0) {
		puts("Invalid session_count");
		return 1;
	}
	if (thread_count > session_count) {
		puts("thread_count must not bigger than session_count");
		return 1;
	}
	if (session_count % thread_count) {
		puts("session_count must times thread_count");
		return 1;
	}

	timeout = atoi(argv[5]);
	if (timeout <= 0) {
		puts("Invalid timeout");
		return 1;
	}

	printf("using libevent %s\n", event_get_version());
	int session_per_thread = session_count / thread_count;
	printf("starting %s to %s:%d with %d threads, %d sessions, %d sessions per thread, timeout=%ds\n",
		   argv[0], ip, port, thread_count, session_count, session_per_thread, timeout);

	sockaddr_in sin = { 0 };
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr(ip);
	sin.sin_port = htons(port);

	std::vector<std::thread> threads;

	for (int i = 1; i < thread_count; i++) {		
		auto base = init_thread(sin, i * thread_count, session_per_thread);
		threads.emplace_back(std::thread([&base]() { event_base_dispatch(base); }));
	}

	auto main_thread_base = init_thread(sin, 0, session_per_thread);
	event_base_dispatch(main_thread_base);

	for (int i = 1; i < thread_count; i++) {
		threads[i].join();
	}

	return 0;
}
