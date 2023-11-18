#ifndef __UV_TCPCLIENT_H__
#define __UV_TCPCLIENT_H__

#pragma once

#include "mybuf.h"
#include "uv_tcp.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct uv_tcpclient_s uv_tcpclient_t;
typedef struct uv_tcpclient_settings_s uv_tcpclient_settings_t;

typedef uv_tcp_parse_result_t (*uv_tcpclient_on_parse_t)(uv_tcpclient_t* client, const char* buf, size_t len, size_t* ate);
typedef void (*uv_tcpclient_on_connect_t)(uv_tcpclient_t* client, int connected);
typedef void (*uv_tcpclient_on_closed_t)(uv_tcpclient_t* client);

int uv_tcpclient_new(uv_tcpclient_t** client, uv_loop_t* loop, uv_tcpclient_settings_t* settings);
int uv_tcpclient_start(uv_tcpclient_t* client, const char* domain, const char* port);
void uv_tcpclient_send(uv_tcpclient_t* client, const char* buf, size_t len);
void uv_tcpclient_stop(uv_tcpclient_t* client);
void uv_tcpclient_free(uv_tcpclient_t* client);

struct uv_tcpclient_settings_s {
    uv_tcpclient_on_connect_t on_connect;
    uv_tcpclient_on_parse_t on_parse;
    uv_tcpclient_on_closed_t on_close;
};

struct uv_tcpclient_s {
    void* data;
    uv_loop_t* loop;
    uv_tcp_t* tcp;
    uv_timer_t* reconn_timer;
    uv_getaddrinfo_t resolver;
    uv_connect_t connect_req;
    uv_tcpclient_settings_t* settings;
    char* domain;
    char* port;
    mybuf_t buf;
    int connected;
    int enable_reconn;
};

#ifdef __cplusplus
}
#endif

#endif
