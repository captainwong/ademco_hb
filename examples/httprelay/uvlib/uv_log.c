#include "uv_log.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <uv.h>

#include "mybuf.h"

static uv_log_level_t g_level = uv_log_level_info;
static char g_logfile[1024] = {0};

uv_log_level_t uv_log_level_from_string(const char* str) {
    if (!str) return uv_log_level_invalid;
#define XX(name, value)            \
    if (strcmp(str, #name) == 0) { \
        return value;              \
    }
    UV_LOG_LEVELS_MAP(XX)
#undef XX

    return uv_log_level_invalid;
}

const char* uv_log_level_to_string(uv_log_level_t level) {
    switch (level) {
#define XX(name, value) \
    case value: {       \
        return #name;   \
    }
        UV_LOG_LEVELS_MAP(XX)
#undef XX
        default:
            return "invalid level";
            break;
    }
}

void uv_log_set_level(uv_log_level_t level) {
    g_level = level;
}

void uv_log_set_log_file(const char* file) {
    if (file) {
        // strlcpy(g_logfile, file, sizeof(g_logfile));
        snprintf(g_logfile, sizeof(g_logfile), "%s", file);
    } else {
        g_logfile[0] = '\0';
    }
}

uv_log_level_t uv_log_get_level() {
    return g_level;
}

long uv_log_get_file_size() {
    uv_fs_t req;
    long size;

    if (g_logfile[0] == '\0') return 0;

    if (uv_fs_stat(uv_default_loop(), &req, g_logfile, NULL) != 0) return -1;
    size = (long)req.statbuf.st_size;
    uv_fs_req_cleanup(&req);
    return size;
}

void uv_log_raw(uv_log_level_t level, const char* msg) {
    size_t off;
    uv_timeval64_t now;
    char buf[64];
    const char* c = "AVDIWEF";
    int log_to_stdout = g_logfile[0] == '\0';
    FILE* fp = NULL;
    time_t t;
    struct tm tm_buf;

    /* separate raw flag from base level */
    int raw = (level & uv_log_level_raw) != 0;
    uv_log_level_t base = level & ~uv_log_level_raw;

    /* only check base level range; raw flag does not affect filtering */
    if (base < g_level || base > uv_log_level_fatal) return;

    fp = log_to_stdout ? stdout : fopen(g_logfile, "a");
    if (!fp) return;

    uv_gettimeofday(&now);

    if (!raw) {
        t = now.tv_sec;
#ifdef _WIN32
        localtime_s(&tm_buf, &t);
#else
        localtime_r(&t, &tm_buf);
#endif
        off = strftime(buf, sizeof(buf), "%b %d %H:%M:%S.", &tm_buf);
        snprintf(buf + off, sizeof(buf) - off, "%03d", (int)(now.tv_usec / 1000));
        fprintf(fp, "%s %c %s\n", buf, c[base], msg);
    } else {
        fprintf(fp, "%s\n", msg);
    }
    fflush(fp);
    if (!log_to_stdout) fclose(fp);
}

void uv_log_hexdump(uv_log_level_t level, const char* buf, size_t len,
                    int show_header, int show_address, int show_ascii, char def_ascii,
                    const char* newline) {
    mybuf_t mybuf;
    uv_log_level_t base = level & ~uv_log_level_raw;
    if (base < g_level || base > uv_log_level_fatal) return;
    mybuf_init(&mybuf);
    mybuf_cat_printf(&mybuf, "hex dump of %p, len=%zu\n", buf, len);
    mybuf_append_hexdump(&mybuf, buf, len, show_header, show_address, show_ascii, def_ascii, newline);
    uv_log_raw(level, mybuf.buf);
    mybuf_clear(&mybuf);
}

static void uv_log_vprintf(uv_log_level_t level, const char* fmt, va_list ap) {
    va_list cpy;
    char msg[1024], *p = msg;
    size_t len = sizeof(msg);
    int l = 0;

    while (1) {
        va_copy(cpy, ap);
        l = vsnprintf(p, len, fmt, cpy);
        va_end(cpy);

        if (l < 0) {
            return;
        } else if ((size_t)l >= len) {
            if (p != msg) free(p);
            len = (size_t)l + 1;
            p = malloc(len);
            if (p == NULL) {
                return;
            }
            continue;
        }
        break;
    }

    uv_log_raw(level, p);
    if (p != msg)
        free(p);
}

void uv_log(uv_log_level_t level, const char* fmt, ...) {
    va_list ap;
    if (level < g_level) return;
    va_start(ap, fmt);
    uv_log_vprintf(level, fmt, ap);
    va_end(ap);
}
