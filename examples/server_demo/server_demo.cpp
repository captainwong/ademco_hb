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
#else  // linux
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#define SOCKET int
#define INVALID_SOCKET -1
#endif

#define DISABLE_JLIB_LOG2
#include <string.h>

#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

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
    size_t rpos;
    size_t wpos;
    char buff[BUFF_SIZE];

    Buffer() { clear(); }
    void clear() { memset(this, 0, sizeof(Buffer)); }
};

SOCKET clientSock = INVALID_SOCKET;
Buffer clientBuffer = {};
std::string clientAcct = {};
ademco_id_t clientAdemcoId = 0;

std::mutex mutex = {};
std::vector<ademco_event_t> evntsWaiting4Send = {};
char pwd[1024] = {};

// 开关量示例：在收到报警时上报报警，在一定时间后自动上报报警恢复，在撤防时上报报警恢复
constexpr auto DEDUP_GAP = 3;        // 3秒内重复报警事件不上报
constexpr auto SWITCH_TIMEOUT = 10;  // 10秒后自动上报3120报警恢复
typedef struct {
    time_t time;
    ademco_event_t event;
} alarm_one_t;
std::unordered_map<ademco_zone_t, alarm_one_t> switch_alarms;  // 开关量报警缓存, zone -> alarm
const ademco_event_t switch_events[] = {
    // 仅处理这些报警事件，其他事件忽略
    EVENT_BURGLAR,
    // ... 有需要自己改
};

void switch_report(ademco_zone_t zone, ademco_event_t event) {
    // 上报报警
    // 这里自己实现上报逻辑
}

// 开关量处理函数，过滤重复报警事件
void switch_filter(ademco_zone_t zone, ademco_event_t event) {
    bool should_handle = false;
    for (const auto& e : switch_events) {
        if (e == event) {
            should_handle = true;
            break;
        }
    }
    if (!should_handle) {
        return;
    }
    auto now = time(nullptr);
    auto it = switch_alarms.find(zone);
    if (it == switch_alarms.end()) {
        switch_alarms[zone] = {now, event};
        switch_report(zone, event);
        return;
    }
    if (now - it->second.time < DEDUP_GAP) {
        return;
    }
    it->second.time = now;
    it->second.event = event;
    switch_report(zone, event);
}

// 开关量清理函数，撤防时上报报警恢复
void swithc_all_clear() {
    for (auto& it : switch_alarms) {
        switch_report(it.first, EVENT_BURGLAR_RECOVER);
    }
    switch_alarms.clear();
}

// 开关量超时处理函数，超时自动上报报警恢复
void switch_check_timeout() {
    auto now = time(nullptr);
    for (auto it = switch_alarms.begin(); it != switch_alarms.end();) {
        if (now - it->second.time > SWITCH_TIMEOUT) {
            switch_report(it->first, EVENT_BURGLAR_RECOVER);
            it = switch_alarms.erase(it);
        } else {
            ++it;
        }
    }
}

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
        if (clientSock != INVALID_SOCKET) {
            return;
        }

        struct sockaddr_in sForeignAddrIn;
        socklen_t nLength = sizeof(struct sockaddr_in);

        fd_set rfd;
        FD_ZERO(&rfd);
        FD_SET(serverSock, &rfd);
        timeval timeout = {1, 0};
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

    auto do_handle = []() -> ademco_parse_result_t {
        ademco_packet_t pkt;
        size_t cb = 0;
        ademco_parse_result_t result = ademco_parse_packet(clientBuffer.buff + clientBuffer.rpos,
                                                           clientBuffer.wpos - clientBuffer.rpos,
                                                           &pkt,
                                                           &cb,
                                                           nullptr);

        switch (result) {
            case ademco_parse_result_t::ADEMCO_PARSE_RESULT_OK: {
                clientBuffer.rpos += cb;
                // printf("id=%s\n", ap.id_.data());
                printf("C:");
                ademco_print(pkt.raw, pkt.raw_len);
                switch (pkt.id) {
                    case AID_ACK:
                        // success
                        break;

                    case AID_NULL:  // reply ack
                    {
                        char ack[1024];
                        size_t len = ademco_make_ack_packet(ack, sizeof(ack), pkt.seq, pkt.acct, 0);
                        printf("S:");
                        ademco_print(ack, len);
                        send(clientSock, (const char*)ack, len, 0);
                        break;
                    }

                    case AID_HB:  // event report
                    case AID_ADM_CID: {
                        clientAcct = pkt.acct;
                        clientAdemcoId = pkt.dat.ademco_id;

                        // reply ack
                        {
                            char ack[1024];
                            int len = ademco_make_ack_packet(ack, sizeof(ack), pkt.seq, pkt.acct, 0);
                            printf("S:");
                            ademco_print(ack, len);
                            send(clientSock, (const char*)ack, len, 0);
                        }

                        // handle event
                        switch (pkt.dat.ademco_event) {
                            case EVENT_BURGLAR:  // 盗警
                            case EVENT_FIRE:     // 火警
                                                 // case ...
                                printf("zone %03d triggered %s\n", pkt.dat.zone, ademco_event_to_string(pkt.dat.ademco_event));
                                break;
                        }

                        // 开关量过滤
                        switch_filter(pkt.dat.zone, pkt.dat.ademco_event);
                        // 撤防时上报报警恢复
                        if (pkt.dat.ademco_event == EVENT_DISARM) {
                            swithc_all_clear();
                        }

                        break;
                    }

                    default:
                        break;
                }
                break;
            }

            case ademco_parse_result_t::ADEMCO_PARSE_RESULT_NOT_ENOUGH:
                // do nothing
                break;

            case ademco_parse_result_t::ADEMCO_PARSE_RESULT_ERROR:
            default:
                // error handle, e.g. clear buff
                clientBuffer.clear();
                break;
        }

        return result;
    };

    auto do_read = [&do_handle]() {
        if (clientSock == INVALID_SOCKET) return;

        timeval tv = {0, 1000000};
        fd_set fd_read;
        FD_ZERO(&fd_read);
        FD_SET(clientSock, &fd_read);
        int nfds = select(int(clientSock + 1), &fd_read, (fd_set*)0, (fd_set*)0, &tv);
        if (nfds <= 0) {
            return;
        }

        int bRead = FD_ISSET(clientSock, &fd_read);
        if (!bRead) {
            return;
        }

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
            close(clientSock);
            clientSock = INVALID_SOCKET;
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
                    clientBuffer.wpos -= clientBuffer.rpos;
                    clientBuffer.rpos = 0;
                    result = do_handle();
                } else {
                    result = do_handle();
                }

                if (result == ademco_parse_result_t::ADEMCO_PARSE_RESULT_NOT_ENOUGH) {
                    break;
                }
            }
        }
    };

    bool running = true;

    std::thread worker([&running, &serverSock, &do_accept, &do_read]() {
        while (running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            do_accept();
            do_read();

            // 开关量检查超时
            switch_check_timeout();

            if (clientSock != INVALID_SOCKET && !evntsWaiting4Send.empty()) {
                std::lock_guard<std::mutex> lg(mutex);
                char buf[1024];
                for (auto e : evntsWaiting4Send) {
                    if (e == EVENT_DISARM) {
                        ademco_xdata_t xdata;
                        ademco_make_xdata(&xdata, ADEMCO_XDATA_LENGTH_FMT_TWO_HEX, ADEMCO_XDATA_TRANSFORM_AS_IS, pwd, 6);
                        int len = ademco_make_hb_packet(buf, sizeof(buf), 1, clientAcct.c_str(), clientAdemcoId, e, 0, 0, &xdata);
                        printf("S:");
                        ademco_print(buf, len);
                        send(clientSock, (const char*)buf, len, 0);
                    } else {
                        int len = ademco_make_hb_packet(buf, sizeof(buf), 1, clientAcct.c_str(), clientAdemcoId, e, 0, 0, nullptr);
                        printf("S:");
                        ademco_print(buf, len);
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
            evntsWaiting4Send.push_back(EVENT_ARM_AWAY);
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
