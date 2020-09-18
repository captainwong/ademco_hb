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

#include <event2/listener.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/thread.h>

#define DISABLE_JLIB_LOG2
#define ENABLE_COMMON_MACHINE_TYPE_TO_STRING
#include <hb_helper.h>

using namespace ademco;
using namespace hb;
using namespace hb::common;

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

std::mutex mutex = {};
int64_t totalBytesRead = 0;
int64_t totalBytesWrite = 0;
int64_t totalMsgRead = 0;
int64_t totalMsgWritten = 0;
int64_t totalEncodeTime = 0;
int64_t totalDecodeTime = 0;

struct Session {
	int fd = 0;
	int thread_id = 0;
	bufferevent* bev = nullptr;
	event* timer = nullptr; // life or heartbeat timer
	int id = 0;
	std::string acct = {};
	AdemcoId ademco_id = 0;
	MachineType type = MachineType::Gprs;
	MachineStatus status = MachineStatus::Arm;
	MachineStatus status1 = MachineStatus::Arm;
	MachineStatus status2 = MachineStatus::Arm;
	MachineStatus status3 = MachineStatus::Arm;
	uint16_t seq = 0;

	uint16_t nextSeq() { if (++seq == 10000) { seq = 1; } return seq; }

	AdemcoPacket packet = {};
	size_t lastTimePacketSize = 0;

	int64_t bytesRead = 0;
	int64_t bytesWritten = 0;
	int64_t msgRead = 0;
	int64_t msgWritten = 0;
	int64_t encodeTime = 0;
	int64_t decodeTime = 0;
};

struct ThreadContext {
	int thread_id = 0;
	event_base* base = nullptr;
	sockaddr_in addr = {};
	int session_start = 0;
	int session_end = 0;

	void connectNext();
};
typedef ThreadContext* ThreadContextPtr;

ThreadContextPtr* threadContexts = nullptr;

void handle_ademco_msg(Session* session, bufferevent* bev)
{
	if (print_data) {
		printf("session #%d recv:%s\n", session->id, session->packet.toString().data());
	}
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
		auto now = std::chrono::steady_clock::now();
		auto res = session->packet.parse(buff, len, cb_commited);
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

void heartbeat_timer_cb(evutil_socket_t fd, short what, void* user_data)
{
	auto session = (Session*)user_data;
	session->timer = event_new(threadContexts[session->thread_id]->base, -1, 0, heartbeat_timer_cb, session);
	if (!session->timer) {
		fprintf(stderr, "create heartbeat_timer failed\n");
		event_base_loopbreak(threadContexts[session->thread_id]->base);
		return;
	}
	struct timeval heartbeat_tv = { timeout, 0 };
	event_add(session->timer, &heartbeat_tv);

	char buf[1024];
	session->lastTimePacketSize = session->packet.make_null(buf, sizeof(buf), session->nextSeq(), session->acct, session->ademco_id);
	evbuffer_add(bufferevent_get_output(session->bev), buf, session->lastTimePacketSize);

	if (print_data) {
		printf("session #%d send:%s\n", session->id, session->packet.toString().data());
	}
}

void timer_cb(evutil_socket_t, short, void* arg)
{
	auto session = (Session*)arg;
	printf("session #%d timeout\n", session->id);
	session->timer = nullptr;
	bufferevent_disable(session->bev, EV_WRITE);
	// SHUT_WR
	shutdown(session->fd, 1);
}

void eventcb(struct bufferevent* bev, short events, void* user_data)
{
	auto session = (Session*)user_data;
	printf("eventcb on session #%d events=%04X\n", session->id, events);
	if (events & BEV_EVENT_CONNECTED) {
		auto ctx = threadContexts[session->thread_id];
		printf("session #%d acct=%s ademco_id=%06X connected fd=%d\n", session->id, session->acct.data(), session->ademco_id, session->fd);
		{
			std::lock_guard<std::mutex> lg(mutex);
			printf("live connections %d\n", ++session_connected);
			if (session_connected == session_count) {
				printf("All connected\n");
			}
		}
		char buf[1024];
		session->lastTimePacketSize = session->packet.make_hb(buf, sizeof(buf),
															  session->nextSeq(), session->acct, session->ademco_id, 0, ademcoEventFromMachineStatus(session->status), 0);
		if (print_data) {
			printf("session #%d send:%s\n", session->id, session->packet.toString().data());
		}
		session->lastTimePacketSize += session->packet.make_hb(buf + session->lastTimePacketSize, sizeof(buf) - session->lastTimePacketSize,
															   session->nextSeq(), session->acct, session->ademco_id, 0, ademcoEventFromMachineType(session->type), 0);
		if (print_data) {
			printf("session #%d send:%s\n", session->id, session->packet.toString().data());
		}
		evbuffer_add(bufferevent_get_output(bev), buf, session->lastTimePacketSize);

		auto base = bufferevent_get_base(bev);
		session->timer = event_new(base, -1, EV_TIMEOUT, timer_cb, session);
		if (!session->timer) {
			fprintf(stderr, "create timer failed\n");
			event_base_loopbreak(base);
			return;
		}
		struct timeval tv = { timeout, 0 };
		event_add(session->timer, &tv);

		if (++ctx->session_start < ctx->session_end) {
			ctx->connectNext();
		}

		return;
	} else if (events & (BEV_EVENT_EOF)) {
		printf("session #%d fd=%d acct=%s ademco_id=%06X closed.\n", session->id, session->fd, session->acct.data(), session->ademco_id);
	} else if (events & BEV_EVENT_ERROR) {
		printf("Got an error on session #%d: %s\n",
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

		--session_connected;
		printf("live connections %d\n", session_connected);
		if (session_connected == 0) {
			printf("All disconnected\n");
			double encodeSpeed = totalEncodeTime * 1.0 / totalMsgWritten;
			double decodeSpeed = totalDecodeTime * 1.0 / totalMsgRead;
			printf("Read %.2f MiB %" PRId64 " packets, Write %.2f MiB %" PRId64 " packets\n"
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

	if (session->timer) {
		event_del(session->timer);
		session->timer = nullptr;
	}
	delete session;
	bufferevent_free(bev);
	//event_base_loopbreak(bufferevent_get_base(bev));	
}

ThreadContext* init_thread(int thread_id, const sockaddr_in& sin, int session_start, int session_per_thread)
{
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
	session->acct = std::string("861234567890") + std::to_string(session_start);
	session->ademco_id = session_start + 1;
	session->seq = 0;
	bufferevent_setcb(bev, readcb, writecb, eventcb, session);
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
	threadContexts = new ThreadContextPtr[thread_count];

	for (int i = 1; i < thread_count; i++) {
		threads.push_back(std::thread([&sin, i, session_per_thread]() {
			auto context = init_thread(i, sin, i * (int)session_per_thread, (int)session_per_thread);
			event_base_dispatch(context->base);
		}));
	}

	auto main_thread_context = init_thread(0, sin, 0, (int)session_per_thread);
	event_base_dispatch(main_thread_context->base);

	for (auto& t : threads) {
		t.join();
	}

	return 0;
}
