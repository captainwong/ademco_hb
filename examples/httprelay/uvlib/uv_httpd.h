#ifndef __UV_HTTPD_H__
#define __UV_HTTPD_H__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "uv_http.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef UV_HTTPD_MAX_BODY_SIZE
#define UV_HTTPD_MAX_BODY_SIZE (10 * 1024 * 1024)  // 10MB
#endif

typedef struct {
    size_t offset;
    size_t len;
} uv_httpd_string_t;

typedef struct {
    uv_httpd_string_t key;
    uv_httpd_string_t value;
} uv_httpd_header_t;

typedef struct {
    size_t n;
    uv_httpd_header_t* headers;
} uv_httpd_headers_t;

typedef struct uv_httpd_request_s uv_httpd_request_t;
typedef struct uv_httpd_client_s uv_httpd_client_t;
typedef struct uv_httpd_server_s uv_httpd_server_t;

struct uv_httpd_request_s {
    char ip[24];       // peer address
    const char* base;  // base address for offset/len
    llhttp_method_t method;
    uv_httpd_string_t url;
    uv_httpd_string_t version;
    uv_httpd_headers_t headers;  // user should NOT free
    uv_httpd_string_t body;
};

typedef void (*on_request_t)(uv_httpd_server_t* server, uv_httpd_client_t* client, uv_httpd_request_t* req);
typedef void (*on_httpd_client_closing_cb_t)(uv_httpd_client_t* client, void* user);

// enable `printf`s, default is disabled
UVLIB_EXPORT_SYMBOL void uv_httpd_enable_printf(int enable);
// return 0 for success, otherwise it is `uv_errno_t`
// if your want to use a existing `uv_loop_t`, pass it by `loop`
// otherwise a new `uv_loop_t` will be created.
UVLIB_EXPORT_SYMBOL int uv_httpd_create(uv_httpd_server_t** server, uv_loop_t* loop, on_request_t on_request);
// call me when you want to stop the server, and then call `uv_run` to continue the stop process.
// after uv_run stopped, you can call `uv_httpd_free` to free the server.
UVLIB_EXPORT_SYMBOL void uv_httpd_stop(uv_httpd_server_t* server);
// Don't just call `uv_httpd_stop(); uv_httpd_free();` to free the server,
// you should call `uv_run()` after `uv_httpd_stop()` to continue the stop process, and then call `uv_httpd_free()` to free the server.
UVLIB_EXPORT_SYMBOL void uv_httpd_free(uv_httpd_server_t* server);
// return 0 for success, otherwise it is uv_errno_t
UVLIB_EXPORT_SYMBOL int uv_httpd_listen(uv_httpd_server_t* server, const char* ip, int port);
// return 0 for success, otherwise it is uv_errno_t
UVLIB_EXPORT_SYMBOL int uv_httpd_write_response(uv_httpd_client_t* client, char* response, size_t len);
UVLIB_EXPORT_SYMBOL void uv_httpd_setup_on_client_closing(uv_httpd_client_t* client, on_httpd_client_closing_cb_t on_closing, void* user);

struct uv_httpd_client_s {
    uv_tcp_t tcp;
    uv_httpd_server_t* server;
    llhttp_t parser;
    on_request_t on_request;
    uv_httpd_request_t req;
    uv_httpd_header_t headers[16];
    void* buf;   // mybuf_t, opaque here
    void* pkt;   // mybuf_t, opaque here
    struct {
        on_httpd_client_closing_cb_t on_closing;
        void* user;
    } user_on_closing;
    uv_httpd_client_t* next;  // linked list in server
};

struct uv_httpd_server_s {
    uv_tcp_t tcp;
    llhttp_settings_t http_settings;
    on_request_t on_request;
    void* data;
    uv_httpd_client_t* clients;  // linked list of active clients
    int nclients;                // count of active clients
};

#ifdef __cplusplus
}
#endif

#endif
