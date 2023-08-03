#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "mybuf.h"
#include "uv_log.h"

void mybuf_init(mybuf_t* buf) {
	buf->buf = buf->mybuf;
	buf->size = 0;
	buf->capacity = MYBUF_DEFAULT_SIZE;
}

size_t mybuf_space(mybuf_t* buf) {
	return buf->capacity - buf->size;
}

void mybuf_reserve(mybuf_t* buf, size_t size) {
	if (mybuf_space(buf) < size) {
		//fprintf(stderr, "WARN: mybuf_t not enough, space=%zu, needed=%zu\n", mybuf_space(buf), size);
		while (mybuf_space(buf) < size) {
			buf->capacity *= 2;
		}
		if (buf->buf == buf->mybuf) {
			buf->buf = (char*)malloc(buf->capacity);
			fatal_if_null(buf->buf);
			memcpy(buf->buf, buf->mybuf, buf->size);
		} else {
			char* tmp = (char*)realloc(buf->buf, buf->capacity);
			fatal_if_null(tmp);
			buf->buf = tmp;
		}
	}
}

void mybuf_append(mybuf_t* buf, const char* data, size_t len) {
	if (mybuf_space(buf) >= len) {
		memcpy(buf->buf + buf->size, data, len);
		buf->size += len;
	} else {
		//fprintf(stderr, "WARN: mybuf_t not enough, space=%zu, needed=%zu\n", mybuf_space(buf), len);
		while (mybuf_space(buf) < len) {
			buf->capacity *= 2;
		}
		if (buf->buf == buf->mybuf) {
			buf->buf = (char*)malloc(buf->capacity);
			fatal_if_null(buf->buf);
			memcpy(buf->buf, buf->mybuf, buf->size);
		} else {
			char* tmp = (char*)realloc(buf->buf, buf->capacity);
			fatal_if_null(tmp);
			buf->buf = tmp;
		}
		memcpy(buf->buf + buf->size, data, len);
		buf->size += len;
	}
}

static void mybuf_cat_vprintf(mybuf_t* buf, const char* fmt, va_list ap) {
	va_list cpy;
	int l;

	while (1) {
		va_copy(cpy, ap);
		l = vsnprintf(buf->buf + buf->size, mybuf_space(buf), fmt, cpy);
		va_end(cpy);
		if (l < 0) {
			uvlog_error("mybuf_cat_vprintf error:%d", l);
			return;
		} else if ((size_t)l >= mybuf_space(buf)) {
			mybuf_reserve(buf, (size_t)l + 1);
			continue;
		} else {
			break;
		}
	}
	buf->size += (size_t)l;
}

void mybuf_cat_printf(mybuf_t* buf, const char* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	mybuf_cat_vprintf(buf, fmt, ap);
	va_end(ap);
}

void mybuf_clear(mybuf_t* buf) {
	if (buf->buf != buf->mybuf) {
		free(buf->buf);
		buf->buf = buf->mybuf;
	}
	buf->size = 0;
	buf->capacity = MYBUF_DEFAULT_SIZE;
}
