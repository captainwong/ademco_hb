#ifdef _WIN32
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#define ioctl ioctlsocket
#define socklen_t int
#define close closesocket
#else // linux
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define SOCKET int
#define INVALID_SOCKET -1
#endif

#define DISABLE_JLIB_LOG2
#include <thread>
#include <vector>
#include <mutex>
#include <string.h>
#include "../../ademco.h"


void op_usage() {
	printf("Press A for Arm, D for Disarm, E for Emergency, Q for Quit\n");
}

void usage(const char* name) {
	printf("Usage: %s listening_port\n", name);
	op_usage();
}

constexpr size_t BUFF_SIZE = 4096;

struct Buffer {
	size_t	rpos;
	size_t	wpos;
	char	buff[BUFF_SIZE];

	Buffer() { clear(); }
	void clear() { memset(this, 0, sizeof(Buffer)); }
};

SOCKET clientSock = INVALID_SOCKET;
Buffer clientBuffer = {};
std::string clientAcct = {};
AdemcoId clientAdemcoId = 0;

std::mutex mutex = {};
std::vector<AdemcoEvent> evntsWaiting4Send = {};
char pwd[1024] = {};

int setNonBlocking(SOCKET socket) {
	u_long lngMode = 1;
	int ret = ioctl(socket, FIONBIO, (u_long*)&lngMode);
	if (ret != 0) {
		fprintf(stderr, "ioctl failed %d\n", ret);
		return ret;
	}
	return ret;
}

int main(int argc, char** argv) {
	usage(argv[0]);

#ifdef _WIN32
	WSADATA wsaData;
	int err = WSAStartup(MAKEWORD(1, 1), &wsaData);
	if (err != 0) {
		printf("init wsa failed %d\n", err);
		abort();
	}
#endif

	int port = 12345;

	if (argc > 1) {
		port = atoi(argv[1]);
	}

	struct sockaddr_in sAddrIn;
	memset(&sAddrIn, 0, sizeof(sAddrIn));
	sAddrIn.sin_family = AF_INET;
	sAddrIn.sin_port = htons(static_cast<u_short>(port));
	sAddrIn.sin_addr.s_addr = INADDR_ANY;

	auto serverSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	int ret = setNonBlocking(serverSock);
	if (ret != 0) {
		return ret;
	}
	ret = bind(serverSock, (struct sockaddr*)&sAddrIn, sizeof(sAddrIn));
	if (ret != 0) {
		fprintf(stderr, "bind failed %d\n", ret);
		return ret;
	}

	ret = listen(serverSock, SOMAXCONN);
	if (ret != 0) {
		fprintf(stderr, "listen failed %d\n", ret);
		return ret;
	}

	printf("Listening on %s:%d\n", inet_ntoa(sAddrIn.sin_addr), ntohs(sAddrIn.sin_port));

	auto do_accept = [&serverSock]() {
		if (clientSock != INVALID_SOCKET) { return; }

		struct sockaddr_in sForeignAddrIn;
		socklen_t nLength = sizeof(struct sockaddr_in);

		fd_set rfd;
		FD_ZERO(&rfd);
		FD_SET(serverSock, &rfd);
		timeval timeout = { 1, 0 };
		int nfds = select((int)(serverSock + 1), &rfd, (fd_set*)0, (fd_set*)0, &timeout);
		printf("do_accept, select ret=%d\n", nfds);
		if (nfds > 0) {
			FD_CLR(serverSock, &rfd);
			clientSock = accept(serverSock, (struct sockaddr*)&sForeignAddrIn, &nLength);
			int ret = setNonBlocking(clientSock);
			if (ret != 0) {
				exit(0);
			}
			printf("Got connection from %s:%d, fd=%d\n", inet_ntoa(sForeignAddrIn.sin_addr), sForeignAddrIn.sin_port, (int)clientSock);
		}
	};

	auto do_handle = []() -> AdemcoParseResult {
		AdemcoPacket pkt; size_t cb = 0;
		AdemcoParseResult result = ademcoPacketParse(clientBuffer.buff + clientBuffer.rpos,
													 clientBuffer.wpos - clientBuffer.rpos,
													 &pkt,
													 &cb,
													 nullptr);

		switch (result) {
		case AdemcoParseResult::RESULT_OK:
		{
			clientBuffer.rpos += cb;
			//printf("id=%s\n", ap.id_.data());
			printf("C:"); ademcoPrint(pkt.raw, pkt.raw_len);
			switch (pkt.id) {
			case AID_ACK:
				// success
				break;

			case AID_NULL: // reply ack				
			{
				char ack[1024];
				size_t len = ademcoMakeAckPacket(ack, sizeof(ack), pkt.seq, pkt.acct, 0);
				printf("S:"); ademcoPrint(ack, len);
				send(clientSock, (const char*)ack, len, 0);
				break;
			}

			case AID_HB: // event report
			case AID_ADM_CID:
			{
				clientAcct = pkt.acct;
				clientAdemcoId = pkt.data.ademcoId;
				// handle event

				// reply ack
				{
					char ack[1024];
					int len = ademcoMakeAckPacket(ack, sizeof(ack), pkt.seq, pkt.acct, 0);
					printf("S:"); ademcoPrint(ack, len);
					send(clientSock, (const char*)ack, len, 0);
				}

				break;
			}

			default:
				break;
			}
			break;
		}

		case AdemcoParseResult::RESULT_NOT_ENOUGH:
			// do nothing
			break;

		case AdemcoParseResult::RESULT_ERROR:
		default:
			// error handle, e.g. clear buff
			clientBuffer.clear();
			break;
		}

		return result;
	};

	auto do_read = [&do_handle]() {
		if (clientSock == INVALID_SOCKET) return;

		timeval tv = { 0, 1000000 };
		fd_set fd_read;
		FD_ZERO(&fd_read);
		FD_SET(clientSock, &fd_read);
		int nfds = select(int(clientSock + 1), &fd_read, (fd_set*)0, (fd_set*)0, &tv);
		if (nfds <= 0) {
			return;
		}

		int bRead = FD_ISSET(clientSock, &fd_read);
		if (!bRead) { return; }

		char* temp = clientBuffer.buff + clientBuffer.wpos;
		size_t dwLenToRead = BUFF_SIZE - clientBuffer.wpos;
		int bytes_transfered = recv(clientSock, (char*)temp, (int)dwLenToRead, 0);

		if (-1 == bytes_transfered) {
			if (EAGAIN == errno) {
				return;
			}
		}

		if (bytes_transfered <= 0) {
			printf("Client %d offline\n", (int)clientSock);
			close(clientSock); clientSock = INVALID_SOCKET;
			clientBuffer.clear();
		} else {
			clientBuffer.wpos += bytes_transfered;
			auto result = do_handle();
			while (1) {
				size_t bytes_not_commited = clientBuffer.wpos - clientBuffer.rpos;
				if (bytes_not_commited == 0) {
					if (clientBuffer.wpos == BUFF_SIZE) {
						clientBuffer.clear();
					}
					break;
				}

				if (clientBuffer.wpos == BUFF_SIZE) {
					memmove(clientBuffer.buff, clientBuffer.buff + clientBuffer.rpos, bytes_not_commited);
					memset(clientBuffer.buff + bytes_not_commited, 0, BUFF_SIZE - bytes_not_commited);
					clientBuffer.wpos -= clientBuffer.rpos; clientBuffer.rpos = 0;
					result = do_handle();
				} else {
					result = do_handle();
				}

				if (result == AdemcoParseResult::RESULT_NOT_ENOUGH) { break; }
			}
		}
	};

	bool running = true;

	std::thread worker([&running, &serverSock, &do_accept, &do_read]() {
		while (running) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			do_accept();
			do_read();

			if (clientSock != INVALID_SOCKET && !evntsWaiting4Send.empty()) {
				std::lock_guard<std::mutex> lg(mutex);
				char buf[1024];
				for (auto e : evntsWaiting4Send) {
					if (e == EVENT_DISARM) {
						AdemcoXDataSegment xdata;
						ademcoMakeXData(&xdata, TWO_HEX, AdemcoXDataTransform::AdemcoXDataTransform_as_is, pwd, 6);
						int len = ademcoMakeHbPacket(buf, sizeof(buf), 1, clientAcct.c_str(), clientAdemcoId, e, 0, 0, &xdata);
						printf("S:"); ademcoPrint(buf, len);
						send(clientSock, (const char*)buf, len, 0);
					} else {
						int len = ademcoMakeHbPacket(buf, sizeof(buf), 1, clientAcct.c_str(), clientAdemcoId, e, 0, 0, nullptr);
						printf("S:"); ademcoPrint(buf, len);
						send(clientSock, (const char*)buf, len, 0);
					}

				}
				evntsWaiting4Send.clear();
			}
		}
	});

	while (1) {
		std::this_thread::sleep_for(std::chrono::milliseconds(200));

		int cmd = getc(stdin);

		if (cmd == 'a' || cmd == 'A') {
			std::lock_guard<std::mutex> lg(mutex);
			evntsWaiting4Send.push_back(EVENT_ARM);
		} else if (cmd == 'd' || cmd == 'D') {
			int ret = 0;
			do {
				printf("Input 6 digit password:");
				ret = scanf("%s", pwd);
			} while (ret != 1 || strlen(pwd) != 6);
			std::lock_guard<std::mutex> lg(mutex);
			evntsWaiting4Send.push_back(EVENT_DISARM);
		} else if (cmd == 'e' || cmd == 'E') {
			std::lock_guard<std::mutex> lg(mutex);
			evntsWaiting4Send.push_back(EVENT_EMERGENCY);
		} else if (cmd == '\r' || cmd == '\n') {
		} else if (cmd == 'q' || cmd == 'Q') {
			running = false;
			worker.join();
			break;
		} else {
			op_usage();
		}
	}

	close(serverSock);
	printf("Bye!");
}
