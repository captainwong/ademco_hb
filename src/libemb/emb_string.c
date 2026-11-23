#include <libemb/emb_string.h>
#include <stdlib.h>

char emb_to_upper(char ch) {
    if ('a' <= ch && ch <= 'z') {
        return ch - ('a' - 'A');
    }
    return ch;
}

char emb_to_lower(char ch) {
    if ('A' <= ch && ch <= 'Z') {
        return ch + ('a' - 'A');
    }
    return ch;
}

int emb_strnicmp(const char* str1, const char* str2, size_t n) {
    size_t i = 0;
    if (!str1 && !str2) return 0;
    if (!str1 || !str2) return -1;

    while (i < n && str1[i] && str2[i]) {
        char c1 = str1[i];
        char c2 = str2[i];
        // convert to upper case
        c1 = emb_to_upper(c1);
        c2 = emb_to_upper(c2);

        if (c1 != c2) {
            return (int)(unsigned char)c1 - (int)(unsigned char)c2;
        }
        i++;
    }
    if (i < n) {
        // one string ended
        return (int)(unsigned char)str1[i] - (int)(unsigned char)str2[i];
    }
    return 0;
}

size_t emb_strnlen(const char* s, size_t maxlen) {
    size_t len = 0;
    while (len < maxlen && s[len] != '\0') {
        len++;
    }
    return len;
}

bool emb_str_starts_with(const char* str, const char* prefix) {
    if (str == NULL || prefix == NULL) {
        return false;
    }
    while (*prefix) {
        if (*str++ != *prefix++) {
            return false;
        }
    }
    return true;
}

bool emb_str_ends_with(const char* str, const char* suffix) {
    if (str == NULL || suffix == NULL) {
        return false;
    }
    const char* str_end = str;
    const char* suffix_end = suffix;
    while (*str_end) {
        str_end++;
    }
    while (*suffix_end) {
        suffix_end++;
    }
    while (suffix_end > suffix) {
        if (str_end == str) {
            return false;  // str比suffix短
        }
        if (*--str_end != *--suffix_end) {
            return false;
        }
    }
    return true;
}

bool emb_str_contains(const char* str, const char* substr) {
    if (str == NULL || substr == NULL) {
        return false;
    }
    // 空串是任意字符串的子串（包括空串自身）
    if (*substr == '\0') {
        return true;
    }
    const char* p = str;
    while (*p) {
        const char* p1 = p;
        const char* p2 = substr;
        while (*p1 && *p2 && *p1 == *p2) {
            p1++;
            p2++;
        }
        if (*p2 == '\0') {
            return true;
        }
        p++;
    }
    return false;
}

uint8_t emb_hex2char(uint8_t h) {
    h &= 0x0F;
    if (h > 9)
        return 'A' + h - 10;
    else
        return '0' + h;
}

uint8_t emb_char2hex(uint8_t c) {
    if ('0' <= c && c <= '9')
        return c - '0';
    else if ('A' <= c && c <= 'F')
        return c - 'A' + 10;
    else if ('a' <= c && c <= 'f')
        return c - 'a' + 10;
    else
        return 0xFF;
}

#ifdef _MSC_VER
// copied from openbsd
// https://github.com/openbsd/src/blob/master/lib/libc/string/strlcpy.c
size_t strlcpy(char* dst, const char* src, size_t dsize) {
    const char* osrc = src;
    size_t nleft = dsize;

    /* Copy as many bytes as will fit. */
    if (nleft != 0) {
        while (--nleft != 0) {
            if ((*dst++ = *src++) == '\0')
                break;
        }
    }

    /* Not enough room in dst, add NUL and traverse rest of src. */
    if (nleft == 0) {
        if (dsize != 0)
            *dst = '\0'; /* NUL-terminate dst */
        while (*src++);
    }

    return (src - osrc - 1); /* count does not include NUL */
}
#endif

char* emb_json_escape_string(const char* str, size_t* out_len) {
    if (!str) {
        if (out_len) *out_len = 0;
        return NULL;
    }

    size_t len = 0;
    const char* p = str;
    while (*p) {
        switch (*p) {
            case '\"':
            case '\\':
            case '\b':
            case '\f':
            case '\n':
            case '\r':
            case '\t':
                len += 2;  // escaped character
                break;
            default:
                if ((unsigned char)*p < 0x20) {
                    len += 6;  // \u00XX
                } else {
                    len += 1;  // normal character
                }
                break;
        }
        p++;
    }

    char* escaped_str = (char*)malloc(len + 1);
    if (!escaped_str) {
        if (out_len) *out_len = 0;
        return NULL;  // allocation failed
    }

    char* dest = escaped_str;
    p = str;
    while (*p) {
        switch (*p) {
            case '\"':
                *dest++ = '\\';
                *dest++ = '\"';
                break;
            case '\\':
                *dest++ = '\\';
                *dest++ = '\\';
                break;
            case '\b':
                *dest++ = '\\';
                *dest++ = 'b';
                break;
            case '\f':
                *dest++ = '\\';
                *dest++ = 'f';
                break;
            case '\n':
                *dest++ = '\\';
                *dest++ = 'n';
                break;
            case '\r':
                *dest++ = '\\';
                *dest++ = 'r';
                break;
            case '\t':
                *dest++ = '\\';
                *dest++ = 't';
                break;
            default:
                if ((unsigned char)*p < 0x20) {
                    dest[0] = '\\';
                    dest[1] = 'u';
                    dest[2] = '0';
                    dest[3] = '0';
                    dest[4] = emb_hex2char(((unsigned char)*p) >> 4);
                    dest[5] = emb_hex2char((unsigned char)*p);
                    dest += 6;
                } else {
                    *dest++ = *p;
                }
                break;
        }
        p++;
    }
    *dest = '\0';

    if (out_len) {
        *out_len = len;
    }
    return escaped_str;
}

void emb_free_json_escaped_string(char* str) {
    free(str);
}

size_t emb_json_escape_string_to_buffer(const char* str, char* buffer, size_t buffer_size) {
    if (!str) {
        if (buffer && buffer_size > 0) {
            buffer[0] = '\0';
        }
        return 0;
    }

    size_t required_size = 0;
    const char* p = str;
    while (*p) {
        switch (*p) {
            case '\"':
            case '\\':
            case '\b':
            case '\f':
            case '\n':
            case '\r':
            case '\t':
                required_size += 2;  // escaped character
                break;
            default:
                if ((unsigned char)*p < 0x20) {
                    required_size += 6;  // \u00XX
                } else {
                    required_size += 1;  // normal character
                }
                break;
        }
        p++;
    }

    if (buffer && buffer_size > 0) {
        size_t to_write = (required_size < buffer_size - 1) ? required_size : buffer_size - 1;
        char* dest = buffer;
        p = str;
        while (*p && to_write > 0) {
            switch (*p) {
                case '\"':
                    if (to_write >= 2) {
                        *dest++ = '\\';
                        *dest++ = '\"';
                        to_write -= 2;
                    } else {
                        to_write = 0;  // Not enough space for escape sequence
                    }
                    break;
                case '\\':
                    if (to_write >= 2) {
                        *dest++ = '\\';
                        *dest++ = '\\';
                        to_write -= 2;
                    } else {
                        to_write = 0;  // Not enough space for escape sequence
                    }
                    break;
                case '\b':
                    if (to_write >= 2) {
                        *dest++ = '\\';
                        *dest++ = 'b';
                        to_write -= 2;
                    } else {
                        to_write = 0;  // Not enough space for escape sequence
                    }
                    break;
                case '\f':
                    if (to_write >= 2) {
                        *dest++ = '\\';
                        *dest++ = 'f';
                        to_write -= 2;
                    } else {
                        to_write = 0;  // Not enough space for escape sequence
                    }
                    break;
                case '\n':
                    if (to_write >= 2) {
                        *dest++ = '\\';
                        *dest++ = 'n';
                        to_write -= 2;
                    } else {
                        to_write = 0;  // Not enough space for escape sequence
                    }
                    break;
                case '\r':
                    if (to_write >= 2) {
                        *dest++ = '\\';
                        *dest++ = 'r';
                        to_write -= 2;
                    } else {
                        to_write = 0;  // Not enough space for escape sequence
                    }
                    break;
                case '\t':
                    if (to_write >= 2) {
                        *dest++ = '\\';
                        *dest++ = 't';
                        to_write -= 2;
                    } else {
                        to_write = 0;  // Not enough space for escape sequence
                    }
                    break;
                default:
                    if ((unsigned char)*p < 0x20) {
                        if (to_write >= 6) {
                            dest[0] = '\\';
                            dest[1] = 'u';
                            dest[2] = '0';
                            dest[3] = '0';
                            dest[4] = emb_hex2char(((unsigned char)*p) >> 4);
                            dest[5] = emb_hex2char((unsigned char)*p);
                            dest += 6;
                            to_write -= 6;
                        } else {
                            to_write = 0;  // Not enough space for escape sequence
                        }
                    } else {
                        *dest++ = *p;
                        to_write -= 1;
                    }
                    break;
            }
            p++;
        }
        *dest = '\0';
    }

    return required_size;
}

bool emb_buf_is_all(const uint8_t* buf, size_t len, uint8_t value) {
    if (!buf || len == 0) {
        return false;
    }
    for (size_t i = 0; i < len; i++) {
        if (buf[i] != value) {
            return false;
        }
    }
    return true;
}
