#ifndef __UV_LOG_H__
#define __UV_LOG_H__

#pragma once

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    uv_log_level_all,
    uv_log_level_debug,
    uv_log_level_info,
    uv_log_level_warn,
    uv_log_level_error,
    uv_log_level_fatal,
    uv_log_level_raw = 1 << 10,  // modifier to log without timestamp
} uv_log_level_t;

void uv_log_set_level(uv_log_level_t level);
void uv_log_set_log_file(const char* file);
uv_log_level_t uv_log_get_level();
void uv_log(uv_log_level_t level, const char* fmt, ...);
void uv_log_raw(uv_log_level_t level, const char* msg);

#define uvlog_all(...) uv_log(uv_log_level_all, __VA_ARGS__)
#define uvlog_debug(...) uv_log(uv_log_level_debug, __VA_ARGS__)
#define uvlog_dline uvlog_debug("%s:%d", __FILE__, __LINE__)
#define uvlog_info(...) uv_log(uv_log_level_info, __VA_ARGS__)
#define uvlog_warn(...) uv_log(uv_log_level_warn, __VA_ARGS__)
#define uvlog_error(...) uv_log(uv_log_level_error, __VA_ARGS__)
#define uvlog_fatal(...) uv_log(uv_log_level_fatal, __VA_ARGS__)

#define warn_on_uv_err(r, msg)                                          \
    do {                                                                \
        if (r) {                                                        \
            uvlog_warn("WARN: %s:%d %s: %d %s %s",                      \
                       __FILE__, __LINE__,                              \
                       (msg), (r), uv_err_name((r)), uv_strerror((r))); \
        }                                                               \
    } while (0);

#define fatal_on_uv_err(r, msg)                                      \
    do {                                                             \
        if ((r)) {                                                   \
            uvlog_fatal("FATAL %s:%d %s: %d %s %s",                  \
                        __FILE__, __LINE__,                          \
                        (msg), r, uv_err_name(r), uv_strerror((r))); \
            exit(r);                                                 \
        }                                                            \
    } while (0);

#define fatal_if_null(p)                             \
    do {                                             \
        if ((p) == NULL) {                           \
            uvlog_fatal("FATAL %s:%d malloc failed", \
                        __FILE__, __LINE__);         \
            abort();                                 \
        }                                            \
    } while (0);

#ifdef __cplusplus
}
#endif

#endif
