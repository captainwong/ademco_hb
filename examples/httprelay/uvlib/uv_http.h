#ifndef __UV_HTTP_H__
#define __UV_HTTP_H__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <uv.h>

#include "llhttp/include/llhttp.h"
#include "uv_log.h"

#ifdef __cplusplus
extern "C" {
#endif

/* @deprecated: internal debug helper, use uv_log_hexdump instead */
UVLIB_EXPORT_SYMBOL void nprintf(const char* msg, size_t len, int newline);

#ifdef __cplusplus
}
#endif

#endif  // !__UV_HTTP_H__
