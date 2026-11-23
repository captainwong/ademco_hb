#ifndef __UV_TCPSERVER_H__
#define __UV_TCPSERVER_H__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <uv.h>

#include "mybuf.h"
#include "uv_tcp.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct uv_tcpserver_s uv_tcpserver_t;
typedef struct uv_tcpserver_client_context_s uv_tcpserver_client_context_t;

// on_connect return value when connected=1:
//   Return 1 = upper layer takes ownership of this client (server will NOT manage it).
//   Return 0 = upper layer has NOT taken ownership, server keeps it in pending list.
// When connected=0, the return value is ignored.
typedef int (*uv_tcpserver_on_connection_t)(uv_tcpserver_client_context_t* client, int connected);
typedef uv_tcp_parse_result_t (*uv_tcpserver_on_parse_t)(uv_tcpserver_client_context_t* client, const char* buf, size_t len, size_t* ate);
typedef void (*uv_tcpserver_on_server_shutdown_t)(uv_tcpserver_t* server);
typedef struct uv_tcpserver_settings_s uv_tcpserver_settings_t;

/**
 * Create a new TCP server.
 * @param settings Must NOT be NULL. All callbacks (on_connect, on_parse, on_server_shutdown) are expected to be set.
 */
UVLIB_EXPORT_SYMBOL int uv_tcpserver_create(uv_tcpserver_t** server, uv_loop_t* loop, uv_tcpserver_settings_t* settings);
UVLIB_EXPORT_SYMBOL int uv_tcpserver_start_listen(uv_tcpserver_t* server, const char* listen_addr, int listen_port);
UVLIB_EXPORT_SYMBOL void uv_tcpserver_stop(uv_tcpserver_t* server);
UVLIB_EXPORT_SYMBOL void uv_tcpserver_free(uv_tcpserver_t* server);
// Close a client. Before closing, on_connect(client, 0) is called so the upper layer can clean up.
UVLIB_EXPORT_SYMBOL void uv_tcpserver_close_cli(uv_tcpserver_client_context_t* cli);
// cli must be diconnected already, safe pattern is call this function in callback `on_connect` with `connected = 0`
UVLIB_EXPORT_SYMBOL void uv_tcpserver_free_cli_ctx(uv_tcpserver_client_context_t* cli);
// Upper layer takes ownership of this client. The server removes it from its pending list.
// Must be called after on_connect(client, 1) returned 0 but before the client is fully authenticated.
UVLIB_EXPORT_SYMBOL void uv_tcpserver_take_cli(uv_tcpserver_client_context_t* cli);
UVLIB_EXPORT_SYMBOL int uv_tcpserver_send_to_cli(uv_tcpserver_client_context_t* cli, const char* buf, size_t len);

struct uv_tcpserver_s {
    void* data;
    uv_tcp_t tcp;
    uv_tcpserver_settings_t* settings;
    uv_tcpserver_client_context_t* pending_head;  // linked list of clients NOT taken by upper layer
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
    uv_tcpserver_client_context_t* next;  // linked list pointer for server's pending list
};

#ifdef __cplusplus
}
#endif

#endif
