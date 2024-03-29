#ifndef __MYBUF_H__
#define __MYBUF_H__

#pragma once

#include "uvlib_config.h"
#include <stddef.h>  // size_t

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
UVLIB_EXPORT_SYMBOL void mybuf_reserve(mybuf_t* buf, size_t size);
UVLIB_EXPORT_SYMBOL void mybuf_append(mybuf_t* buf, const char* data, size_t len);

/*
 * append hex dump to mybuf
 * show_header: show 00~0F address header, default is 0
 * show_address: show 0000~FFFF address on left
 * show_ascii: show ascii on right if can print, default is 0
 * def_ascii: when show_ascii but can't print, show def_ascii instead, default is '.'
 * newline: default is "\n"
 */
UVLIB_EXPORT_SYMBOL void mybuf_append_hexdump(mybuf_t* buf, const char* data, size_t len,
                                              int show_header, int show_address, int show_ascii, char def_ascii,
                                              const char* newline);

#ifdef __GNUC__
UVLIB_EXPORT_SYMBOL void mybuf_cat_printf(mybuf_t* buf, const char* fmt, ...)
                                          __attribute__((format(printf, 2, 3)));
#else
UVLIB_EXPORT_SYMBOL void mybuf_cat_printf(mybuf_t* buf, const char* fmt, ...);
#endif

UVLIB_EXPORT_SYMBOL void mybuf_clear(mybuf_t* buf);
// safe to call me when data in buf
UVLIB_EXPORT_SYMBOL void mybuf_clear_append(mybuf_t* buf, const char* data, size_t len);

// call mybuf_free to free string
UVLIB_EXPORT_SYMBOL char* mybuf_to_string(mybuf_t* buf);
UVLIB_EXPORT_SYMBOL void mybuf_free(char* str);

#ifdef __cplusplus
}
#endif

#endif
