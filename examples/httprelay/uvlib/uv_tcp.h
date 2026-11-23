#ifndef __UV_TCP_H__
#define __UV_TCP_H__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <uv.h>
#include "libuvlib_config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    uv_tcp_parse_ok = 0,
    uv_tcp_parse_not_enough,
    uv_tcp_parse_error,
} uv_tcp_parse_result_t;

UVLIB_EXPORT_SYMBOL int getpeeraddr(uv_tcp_t* tcp, char* ip, size_t len, uint16_t* port);

#ifdef __cplusplus
}
#endif

#endif
