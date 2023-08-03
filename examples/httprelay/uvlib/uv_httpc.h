// uv http client

#ifndef __UV_HTTPC_H__
#define __UV_HTTPC_H__

#pragma once

#include <uv.h>
#include "mybuf.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct uv_httpc_s uv_httpc_t;

typedef struct uv_httpc_headers_s {
	char* data;
	struct uv_httpc_headers_s* next;
}uv_httpc_headers_t;

typedef enum {
	uv_httpc_ok,
	uv_httpc_resolve_failed,
	uv_httpc_connection_failed,
	uv_httpc_connection_reset,
	uv_httpc_connection_timeout,
	uv_httpc_parse_failed,
}uv_httpc_code;

typedef struct uv_httpc_response_s {
	uv_httpc_code code;
	int uvcode;
	uv_httpc_headers_t* headers;
	int http_status;
	char* body;
	size_t len;
}uv_httpc_response_t;

typedef void(*uv_httpc_on_response_t)(uv_httpc_t* httpc, uv_httpc_response_t* res);

int uv_httpc_create(uv_httpc_t** httpc, uv_loop_t* loop, const char* host, const char* port, 
					uv_httpc_on_response_t on_response);
int uv_httpc_post(uv_httpc_t* httpc, const char* url, uv_httpc_headers_t* headers, 
				  const char* data, size_t len, int timeout);
int uv_httpc_stop(uv_httpc_t* httpc);
void uv_httpc_free(uv_httpc_t* httpc);
uv_httpc_headers_t* uv_httpc_headers_append(uv_httpc_headers_t* headers, char* data);
uv_httpc_headers_t* uv_httpc_headers_append_nodup(uv_httpc_headers_t* headers, char* data);
uv_httpc_headers_t* uv_httpc_headers_dup(uv_httpc_headers_t* headers);
void uv_httpc_headers_free_all(uv_httpc_headers_t* headers, int freedata);

#ifdef WEUV_TEST
int uv_httpc_test_main(int argc, char** argv);
#endif


#ifdef __cplusplus
}
#endif

#endif
