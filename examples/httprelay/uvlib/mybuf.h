#ifndef __MYBUF_H__
#define __MYBUF_H__

#pragma once

#include <stddef.h> // size_t

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MYBUF_DEFAULT_SIZE
#define MYBUF_DEFAULT_SIZE 4096
#endif

typedef struct {
	char* buf;
	char mybuf[MYBUF_DEFAULT_SIZE];
	size_t size, capacity;
}mybuf_t;

void mybuf_init(mybuf_t* buf);
size_t mybuf_space(mybuf_t* buf);
void mybuf_reserve(mybuf_t* buf, size_t size);
void mybuf_append(mybuf_t* buf, const char* data, size_t len);

#ifdef __GNUC__
void mybuf_cat_printf(mybuf_t* buf, const char* fmt, ...)
	__attribute__((format(printf, 2, 3)));
#else
void mybuf_cat_printf(mybuf_t* buf, const char* fmt, ...);
#endif

void mybuf_clear(mybuf_t* buf);

#ifdef __cplusplus
}
#endif

#endif
