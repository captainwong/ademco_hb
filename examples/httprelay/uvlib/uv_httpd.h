#ifndef __UV_HTTPD_H__
#define __UV_HTTPD_H__

#pragma once

#include <uv.h>
#include "llhttp/include/llhttp.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	size_t offset;
	size_t len;
}uv_httpd_string_t;

typedef struct {
	uv_httpd_string_t key;
	uv_httpd_string_t value;
}uv_httpd_header_t;

typedef struct {
	size_t n;
	uv_httpd_header_t* headers; 
}uv_httpd_headers_t;

typedef struct {
	char ip[24]; // peer address
	const char* base; // base address for offset/len
	uv_httpd_string_t remote; // remote address ip:port
	llhttp_method_t method;
	uv_httpd_string_t url;
	uv_httpd_string_t version;
	uv_httpd_headers_t headers; // user should NOT free
	uv_httpd_string_t body;
}uv_httpd_request_t;

typedef struct uv_httpd_client_s uv_httpd_client_t;
typedef struct uv_httpd_server_s uv_httpd_server_t;

typedef void(*on_request_t)(uv_httpd_server_t* server, uv_httpd_client_t* client, uv_httpd_request_t* req);

void nprintf(const char* msg, size_t len, int newline);
int string_ncmp(const char* s1, size_t len1, const char* s2, size_t len2);
int string0_ncmp(const char* s1, const char* s2, size_t len2);
int string_nicmp(const char* s1, size_t len1, const char* s2, size_t len2);
int string0_nicmp(const char* s1, const char* s2, size_t len2);

// enable `printf`s, default is disabled
void uv_httpd_enable_printf(int enable);
// return 0 for success, otherwise it is `uv_errno_t`
// if your want to use a existing `uv_loop_t`, pass it by `loop`
// otherwise a new `uv_loop_t` will be created.
int uv_httpd_create(uv_httpd_server_t** server, uv_loop_t* loop, on_request_t on_request);
void uv_httpd_stop(uv_httpd_server_t* server);
void uv_httpd_free(uv_httpd_server_t* server);
// return 0 for success, otherwise it is uv_errno_t
int uv_httpd_listen(uv_httpd_server_t* server, const char* ip, int port);
// return 0 for success, otherwise it is uv_errno_t
int uv_httpd_write_response(uv_httpd_client_t* client, char* response, size_t len);


struct uv_httpd_server_s {
	uv_tcp_t tcp;
	llhttp_settings_t http_settings;
	on_request_t on_request;
	void* data;
};

#ifdef __cplusplus
}
#endif

#endif
