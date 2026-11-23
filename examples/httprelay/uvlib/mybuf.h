#ifndef __MYBUF_H__
#define __MYBUF_H__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stddef.h>  // size_t

#include "libuvlib_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MYBUF_DEFAULT_SIZE
#define MYBUF_DEFAULT_SIZE 4096
#endif

#if MYBUF_DEFAULT_SIZE < 8
#error "MYBUF_DEFAULT_SIZE must >= 8"
#endif

typedef struct {
    char* buf;
    char mybuf[MYBUF_DEFAULT_SIZE];
    size_t size, capacity;
} mybuf_t;

UVLIB_EXPORT_SYMBOL void mybuf_init(mybuf_t* buf);
UVLIB_EXPORT_SYMBOL size_t mybuf_space(mybuf_t* buf);
// remember to call `mybuf_clear` incase of memory leak
UVLIB_EXPORT_SYMBOL void mybuf_reserve(mybuf_t* buf, size_t size);
// remember to call `mybuf_clear` incase of memory leak
UVLIB_EXPORT_SYMBOL void mybuf_append(mybuf_t* buf, const char* data, size_t len);

/*
 * @brief append hex dump to mybuf
 * @param show_header: show 00-0F address header, default is 0
 * @param show_address: show 0000-FFFF address on left
 * @param show_ascii: show ascii on right if can print, default is 0
 * @param def_ascii: when show_ascii but can't print, show def_ascii instead, default is '.'
 * @param newline: default is "\n"
 * @note remember to call `mybuf_clear` incase of memory leak
 */
UVLIB_EXPORT_SYMBOL void mybuf_append_hexdump(mybuf_t* buf, const char* data, size_t len,
                                              int show_header, int show_address, int show_ascii, char def_ascii,
                                              const char* newline);

#ifdef __GNUC__
// remember to call `mybuf_clear` incase of memory leak
UVLIB_EXPORT_SYMBOL void mybuf_cat_printf(mybuf_t* buf, const char* fmt, ...)
    __attribute__((format(printf, 2, 3)));
#else
// remember to call `mybuf_clear` incase of memory leak
UVLIB_EXPORT_SYMBOL void mybuf_cat_printf(mybuf_t* buf, const char* fmt, ...);
#endif

/**
 * @brief alloc a string with printf format
 * @param fmt printf format string
 * @return allocated string, remember to call `mybuf_free` or `free` to free it
 */
#ifdef __GNUC__
UVLIB_EXPORT_SYMBOL char* mybuf_printf_alloc(const char* fmt, ...) __attribute__((format(printf, 1, 2)));
#else
UVLIB_EXPORT_SYMBOL char* mybuf_printf_alloc(const char* fmt, ...);
#endif

// clear mybuf, also free the buf if it is not mybuf[MYBUF_DEFAULT_SIZE]
UVLIB_EXPORT_SYMBOL void mybuf_clear(mybuf_t* buf);
// safe to call me when data in buf, remember to call `mybuf_clear` incase of memory leak
UVLIB_EXPORT_SYMBOL void mybuf_clear_append(mybuf_t* buf, const char* data, size_t len);

// call `mybuf_free` to free the returned string
UVLIB_EXPORT_SYMBOL char* mybuf_to_string(mybuf_t* buf);
UVLIB_EXPORT_SYMBOL void mybuf_free(char* str);

#ifdef __cplusplus
}
#endif

#endif
