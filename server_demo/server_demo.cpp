#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#define DISABLE_JLIB_LOG2

#include <ademco_packet.h>
#include <ademco_detail.h>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#include <thread>
#include <vector>
#include <mutex>

using namespace ademco;

void usage(const char* name)
{
	printf("Usage: %s listening_port\nWhile connection established, press A for Arm, D for Disarm, Q for Quit\n", name);
}

constexpr size_t BUFF_SIZE = 4096;

struct Buffer
{
	unsigned int	rpos;
	unsigned int	wpos;
	char			buff[BUFF_SIZE];

	Buffer() { clear(); }
	void clear() { memset(this, 0, sizeof(Buffer)); }
};

SOCKET clientSock = INVALID_SOCKET;
Buffer clientBuffer = {};
std::string clientAcct = {};
size_t clientAdemcoId = 0;

std::mutex mutex = {};
std::vector<ADEMCO_EVENT> evntsWaiting4Send = {};

int main(int argc, char** argv)
{
	usage(argv[0]);	

	WSADATA wsaData;
	int err = WSAStartup(MAKEWORD(1, 1), &wsaData);
	if (err != 0) {
		printf("init wsa failed %d\n", err);
		abort();
	}

	int port = 12345;

	if (argc > 1) {
		port = atoi(argv[1]);
	}

	struct sockaddr_in sAddrIn;
	memset(&sAddrIn, 0, sizeof(sAddrIn));
	sAddrIn.sin_family = AF_INET;
	sAddrIn.sin_port = htons(static_cast<u_short>(port));
	sAddrIn.sin_addr.S_un.S_addr = ADDR_ANY;


	auto serverSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	u_long lngMode = 1;
	ioctlsocket(serverSock, FIONBIO, (u_long FAR*) & lngMode);
	bind(serverSock, (struct sockaddr*) & sAddrIn, sizeof(sAddrIn));
	listen(serverSock, SOMAXCONN);

	auto do_accept = [&serverSock]() {
		if (clientSock != INVALID_SOCKET) { return; }

		struct sockaddr_in sForeignAddIn;
		int nLength = sizeof(struct sockaddr_in);

		fd_set rfd;
		FD_ZERO(&rfd);
		FD_SET(serverSock, &rfd);
		timeval timeout = { 1, 0 };
		int nfds = select(1, &rfd, (fd_set*)0, (fd_set*)0, &timeout);
		if (nfds > 0) {
			FD_CLR(serverSock, &rfd);
			clientSock = accept(serverSock, (struct sockaddr*) & sForeignAddIn, &nLength);
		}

	};

	auto do_handle = []() -> ademco::ParseResult {
		AdemcoPacket ap; size_t dwBytesCommited = 0;
		ParseResult result = ap.parse(clientBuffer.buff + clientBuffer.rpos,
									  clientBuffer.wpos - clientBuffer.rpos,
									  dwBytesCommited);

		switch (result) {
		case ademco::ParseResult::RESULT_OK:
		{
			clientBuffer.rpos += dwBytesCommited;
			//printf("id=%s\n", ap.id_.data());
			printf("C:%s\n", ap.toString().data());
			switch (ap.id_.eid_) {
			case AdemcoId::id_ack:
				// success
				break;

			case AdemcoId::id_null: // reply ack				
			{
				char ack[1024];
				auto len = ap.make_ack(ack, sizeof(ack), ap.seq_.value_, ap.acct_.acct(), ap.ademcoData_.ademco_id_);
				printf("S:%s\n", ap.toString().data());
				send(clientSock, ack, len, 0);
				break;
			}

			case AdemcoId::id_hb: // event report
			{
				clientAcct = ap.acct_.acct();
				clientAdemcoId = ap.ademcoData_.ademco_id_;
				// handle event
				//printf("%s\n", ap.ademcoData_.toString().data());

				// reply ack
				{
					char ack[1024];
					auto len = ap.make_ack(ack, sizeof(ack), ap.seq_.value_, ap.acct_.acct(), ap.ademcoData_.ademco_id_);
					printf("S:%s\n", ap.toString().data());
					// send to machine via network
					send(clientSock, ack, len, 0);
				}

				break;
			}

			default:
				break;
			}
			break;
		}

		case ademco::ParseResult::RESULT_NOT_ENOUGH:
			// do nothing
			break;

		case ademco::ParseResult::RESULT_DATA_ERROR:
		default:
			// error handle, e.g. clear buff
			clientBuffer.clear();
			break;
		}

		return result;
	};

	auto do_read = [&do_handle]() {
		if (clientSock == INVALID_SOCKET) return;

		timeval tv = { 0, 0 };
		fd_set fd_read;
		FD_ZERO(&fd_read);
		FD_SET(clientSock, &fd_read);

		BOOL bRead = FD_ISSET(clientSock, &fd_read);
		if (!bRead) { return; }

		char* temp = clientBuffer.buff + clientBuffer.wpos;
		DWORD dwLenToRead = BUFF_SIZE - clientBuffer.wpos;
		int bytes_transfered = recv(clientSock, temp, dwLenToRead, 0);

		if (SOCKET_ERROR == bytes_transfered) {
			if (WSAEWOULDBLOCK == WSAGetLastError()) {
				return;
			}
		}

		if (bytes_transfered <= 0) {
			printf("client offline\n");
			closesocket(clientSock);
			clientBuffer.clear();
		} else {
			clientBuffer.wpos += bytes_transfered;			
			auto result = do_handle();
			while (1) {
				unsigned int bytes_not_commited = clientBuffer.wpos - clientBuffer.rpos;
				if (bytes_not_commited == 0) { 
					if (clientBuffer.wpos == BUFF_SIZE) { 
						clientBuffer.clear(); 
					} 
					break;
				}

				if (clientBuffer.wpos == BUFF_SIZE) {
					memmove_s(clientBuffer.buff, BUFF_SIZE, clientBuffer.buff + clientBuffer.rpos, bytes_not_commited);
					memset(clientBuffer.buff + bytes_not_commited, 0, BUFF_SIZE - bytes_not_commited);
					clientBuffer.wpos -= clientBuffer.rpos; clientBuffer.rpos = 0;
					result = do_handle();
				} else { 
					result = do_handle();
				}

				if (result == ademco::ParseResult::RESULT_NOT_ENOUGH) { break; }
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
				AdemcoPacket ap;
				for (auto e : evntsWaiting4Send) {
					auto len = ap.make_hb(buf, sizeof(buf), 1, clientAcct, clientAdemcoId, 0, e, 0);
					printf("S:%s\n", ap.toString().data());
					send(clientSock, buf, len, 0);
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
			std::lock_guard<std::mutex> lg(mutex);
			evntsWaiting4Send.push_back(EVENT_DISARM);
		} else if (cmd == '\r' || cmd == '\n') {
		} else if (cmd == 'q' || cmd == 'Q') {
			running = false;
			worker.join();
			break;
		} else {
			printf("Press A for Arm, D for Disarm, Q for Quit\n");
		}
	}

	printf("Bye!");
}
