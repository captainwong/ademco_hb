#include "uv_http.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "uv_log.h"

void nprintf(const char* at, size_t len, int newline) {
    char buf[4096];
    char* p = buf;
    if (len < sizeof buf) {
        p = buf;
    } else {
        p = malloc(len + 1);
        fatal_if_null(p);
    }
    memcpy(p, at, len);
    p[len] = '\0';
    printf("%s", p);
    if (newline)
        printf("\n");

    if (p != buf) {
        free(p);
    }

    // uv_log_hexdump(uv_log_level_info, at, len, 1, 1, 1, '.', "\n");
    // if (newline) {
    //     printf("\n");
    // }
}
