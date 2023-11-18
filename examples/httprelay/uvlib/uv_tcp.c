#include "uv_tcp.h"

#include "uv_log.h"

int getpeeraddr(uv_tcp_t* tcp, char* ip, size_t len, uint16_t* port) {
    struct sockaddr_in addr;
    int addrlen = sizeof(addr);
    int r = uv_tcp_getpeername(tcp, (struct sockaddr*)&addr, &addrlen);
    if (r) {
        warn_on_uv_err(r, "uv_tcp_getpeername");
        return r;
    }
    r = uv_ip4_name(&addr, ip, len);
    if (r) {
        warn_on_uv_err(r, "uv_ip4_name");
    }
    if (len)
        ip[len - 1] = '\0';

    if (port) {
        *port = ntohs(addr.sin_port);
    }

    return r;
}
