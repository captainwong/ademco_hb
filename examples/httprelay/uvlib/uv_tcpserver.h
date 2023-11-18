#ifndef __UV_TCPSERVER_H__
#define __UV_TCPSERVER_H__

#pragma once

#include <uv.h>

#include "mybuf.h"
#include "uv_tcp.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct uv_tcpserver_s uv_tcpserver_t;
typedef struct uv_tcpserver_client_context_s uv_tcpserver_client_context_t;

typedef void (*uv_tcpserver_on_connection_t)(uv_tcpserver_client_context_t* client, int connected);
typedef uv_tcp_parse_result_t (*uv_tcpserver_on_parse_t)(uv_tcpserver_client_context_t* client, const char* buf, size_t len, size_t* ate);
typedef void (*uv_tcpserver_on_server_shutdown_t)(uv_tcpserver_t* server);
typedef struct uv_tcpserver_settings_s uv_tcpserver_settings_t;

int uv_tcpserver_create(uv_tcpserver_t** server, uv_loop_t* loop, uv_tcpserver_settings_t* settings);
int uv_tcpserver_start_listen(uv_tcpserver_t* server, const char* listen_addr, int listen_port);
void uv_tcpserver_stop(uv_tcpserver_t* server);
void uv_tcpserver_free(uv_tcpserver_t* server);
void uv_tcpserver_close_cli(uv_tcpserver_client_context_t* cli);
// cli must be diconnected already, safe pattern is call this function in callback `on_connect` with `connected = 0`
void uv_tcpserver_free_cli_ctx(uv_tcpserver_client_context_t* cli);
int uv_tcpserver_send_to_cli(uv_tcpserver_client_context_t* cli, const char* buf, size_t len);

struct uv_tcpserver_s {
    void* data;
    uv_tcp_t tcp;
    uv_tcpserver_settings_t* settings;
};

struct uv_tcpserver_settings_s {
    uv_tcpserver_on_connection_t on_connect;
    uv_tcpserver_on_parse_t on_parse;
    uv_tcpserver_on_server_shutdown_t on_server_shutdown;
};

struct uv_tcpserver_client_context_s {
    void* data;
    uv_tcpserver_t* server;
    uv_tcp_t tcp;
    mybuf_t buf;
};

#ifdef __cplusplus
}
#endif

#endif
