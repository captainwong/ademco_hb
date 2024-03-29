#ifndef __UV_LOG_H__
#define __UV_LOG_H__

#pragma once

#include "uvlib_config.h"
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UV_LOG_LEVELS_MAP(XX) \
    XX(all, 0)                \
    XX(verbose, 1)            \
    XX(debug, 2)              \
    XX(info, 3)               \
    XX(warn, 4)               \
    XX(error, 5)              \
    XX(fatal, 6)

typedef enum {
#define XX(name, value) uv_log_level_##name = value,
    UV_LOG_LEVELS_MAP(XX)
#undef XX
    // modifier to log without timestamp
    uv_log_level_raw = 1 << 10,
    // invalid
    uv_log_level_invalid = 1 << 11,
} uv_log_level_t;

UVLIB_EXPORT_SYMBOL uv_log_level_t uv_log_level_from_string(const char* str);
UVLIB_EXPORT_SYMBOL const char* uv_log_level_to_string(uv_log_level_t level);
UVLIB_EXPORT_SYMBOL void uv_log_set_level(uv_log_level_t level);
UVLIB_EXPORT_SYMBOL void uv_log_set_log_file(const char* file);
UVLIB_EXPORT_SYMBOL uv_log_level_t uv_log_get_level();
UVLIB_EXPORT_SYMBOL void uv_log(uv_log_level_t level, const char* fmt, ...);
UVLIB_EXPORT_SYMBOL void uv_log_raw(uv_log_level_t level, const char* msg);

/**
 * log hex dump
 * show_header: show 00~0F address header, default is 0
 * show_address: show 0000~FFFF address on left
 * show_ascii: show ascii on right if can print, default is 0
 * def_ascii: when show_ascii but can't print, show def_ascii instead, default is '.'
 * newline: default is "\n"
 */
UVLIB_EXPORT_SYMBOL void uv_log_hexdump(uv_log_level_t level, const char* buf, size_t len,
                                        int show_header, int show_address, int show_ascii, char def_ascii,
                                        const char* newline);

#define uvlog_all(...) uv_log(uv_log_level_all, __VA_ARGS__)
#define uvlog_verbose(...) uv_log(uv_log_level_verbose, __VA_ARGS__)
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
