#include "mybuf.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
        // fprintf(stderr, "WARN: mybuf_t not enough, space=%zu, needed=%zu\n", mybuf_space(buf), size);
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
        // fprintf(stderr, "WARN: mybuf_t not enough, space=%zu, needed=%zu\n", mybuf_space(buf), len);
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

void mybuf_append_hexdump(mybuf_t* buf, const char* data, size_t len,
                          int show_header, int show_address, int show_ascii, char def_ascii,
                          const char* newline) {
    size_t addr = 0;
    if (!newline) {
        newline = "\n";
    }
    if (show_header) {
        if (show_address)
            mybuf_cat_printf(buf, "ADDR | ");
        mybuf_cat_printf(buf, "00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E OF");
        if (show_ascii)
            mybuf_cat_printf(buf, " | 0123456789ABCDEF");
        mybuf_cat_printf(buf, "%s", newline);

        if (show_address)
            mybuf_cat_printf(buf, "-----+------------------------------------------------");
        else
            mybuf_cat_printf(buf, "---------------------------------------------------");
        if (show_ascii)
            mybuf_cat_printf(buf, "-+-----------------");
        mybuf_cat_printf(buf, "%s", newline);
    }
    if (def_ascii < 32 || def_ascii > 126) {
        def_ascii = '.';  // default ascii
    }
    for (size_t i = 0; i < len; i++) {
        if (show_address) {
            if (i % 16 == 0) {
                mybuf_cat_printf(buf, "%04zX | ", addr);
                addr += 16;
            }
        }
        mybuf_cat_printf(buf, "%02X ", (unsigned char)data[i]);
        if (i % 16 == 15 || i == len - 1) {
            if (show_ascii) {
                if (i % 16 != 15) {
                    for (size_t j = 0; j < 16 - (i % 16) - 1; j++) {
                        mybuf_cat_printf(buf, "   ");
                    }
                }
                mybuf_cat_printf(buf, "| ");
                for (size_t j = i - (i % 16); j <= i; j++) {
                    if (data[j] >= 32 && data[j] <= 126) {
                        mybuf_cat_printf(buf, "%c", data[j]);
                    } else {
                        mybuf_cat_printf(buf, "%c", def_ascii);
                    }
                }
                mybuf_cat_printf(buf, "%s", newline);
            } else {
                mybuf_cat_printf(buf, "%s", newline);
            }
        }
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

void mybuf_clear_append(mybuf_t* buf, const char* data, size_t len) {
    mybuf_t tmp;
    mybuf_init(&tmp);
    mybuf_append(&tmp, data, len);
    mybuf_clear(buf);
    mybuf_append(buf, tmp.buf, tmp.size);
    mybuf_clear(&tmp);
}

char* mybuf_to_string(mybuf_t* buf) {
    size_t len = buf->size;
    if (len == 0) {
        char* str = (char*)malloc(1);
        fatal_if_null(str);
        str[0] = '\0';
        return str;
    }
    if (buf->buf[len - 1] == '\0') {
        len--;
    }
    char* str = (char*)malloc(len + 1);
    fatal_if_null(str);
    memcpy(str, buf->buf, len);
    str[len] = '\0';
    return str;
}

void mybuf_free(char* str) {
    free(str);
}
