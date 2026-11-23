#include "uv_httpc.h"

#include <assert.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "llhttp/include/llhttp.h"
#include "uv_log.h"

#define DEFAULT_BUFF_SIZE 1024

#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif

struct uv_httpc_s {
    void* data;
    int stopped;
    uv_tcp_t tcp;
    uv_timer_t* timer;
    uv_getaddrinfo_t resolver;
    struct addrinfo* addrinfo;
    uv_connect_t connect_req;
    uv_httpc_on_response_t on_response;
    char* host;
    char* port;
    char* method;
    char* url;
    uv_httpc_headers_t* headers;
    char* http_data;
    size_t len;
    int timeout;
    llhttp_t parser;
    llhttp_settings_t http_settings;

    mybuf_t buf;
    uv_httpc_response_t response;
};

typedef struct {
    uv_write_t req;
    uv_buf_t uvbuf;
    mybuf_t buf;
} write_req_t;

static void init_response(uv_httpc_response_t* response) {
    response->code = uv_httpc_ok;
    response->uvcode = 0;
    response->headers = NULL;
    response->http_status = 0;
    response->body = NULL;
    response->len = 0;
}

static void reset_response(uv_httpc_response_t* response) {
    response->code = uv_httpc_ok;
    response->uvcode = 0;
    if (response->headers) {
        uv_httpc_headers_free_all(response->headers, 1);
        response->headers = NULL;
    }
    response->http_status = 0;
    if (response->body) {
        free(response->body);
        response->body = NULL;
    }
    response->len = 0;
}

static void generic_on_response(uv_httpc_t* httpc, uv_httpc_code code, int uvcode);

/*************************** llhttp callback functions ****************/

#ifdef _MSC_VER
static char* strndup(const char* at, size_t len) {
    char* p = malloc(len + 1);
    fatal_if_null(p);
    memcpy(p, at, len);
    p[len] = '\0';
    return p;
}
#endif

static void headers_append_key(uv_httpc_headers_t** headers, const char* key, size_t len) {
    uv_httpc_headers_t* header = *headers;
    while (header && header->next) {
        header = header->next;
    }
    header = uv_httpc_headers_append_nodup(header, strndup(key, len));
    if (!*headers)
        *headers = header;
}

static void headers_append_value(uv_httpc_headers_t** headers, const char* value, size_t len) {
    uv_httpc_headers_t* header = *headers;
    while (header && header->next) {
        header = header->next;
    }
    if (header) {
        size_t keylen = strlen(header->data);
        char* p = malloc(keylen + len + 2);
        fatal_if_null(p);
        memcpy(p, header->data, keylen);
        p[keylen] = ':';
        memcpy(p + keylen + 1, value, len);
        p[keylen + len + 1] = '\0';
        free(header->data);
        header->data = p;
    } else {
        assert(0);
    }
}

static int on_message_begin(llhttp_t* llhttp) {
    uv_httpc_t* httpc = llhttp->data;
    reset_response(&httpc->response);
    return 0;
}

static int on_header_field(llhttp_t* llhttp, const char* at, size_t length) {
    uv_httpc_t* httpc = llhttp->data;
    headers_append_key(&httpc->response.headers, at, length);
    return 0;
}

static int on_header_value(llhttp_t* llhttp, const char* at, size_t length) {
    uv_httpc_t* httpc = llhttp->data;
    headers_append_value(&httpc->response.headers, at, length);
    return 0;
}

static int on_headers_complete(llhttp_t* llhttp) {
    UNUSED(llhttp);
    return 0;
}

static int on_body(llhttp_t* llhttp, const char* at, size_t length) {
    uv_httpc_t* httpc = llhttp->data;
    httpc->response.body = strndup(at, length);
    httpc->response.len = length;
    return 0;
}

static int on_message_complete(llhttp_t* llhttp) {
    uv_httpc_t* httpc = llhttp->data;
    generic_on_response(httpc, uv_httpc_ok, 0);
    reset_response(&httpc->response);
    return 0;
}

static int on_status_complete(llhttp_t* llhttp) {
    uv_httpc_t* httpc = llhttp->data;
    httpc->response.http_status = llhttp->status_code;
    return 0;
}

static void setup_default_llhttp_settings(llhttp_settings_t* settings) {
    memset(settings, 0, sizeof(*settings));
    settings->on_message_begin = on_message_begin;
    settings->on_header_field = on_header_field;
    settings->on_header_value = on_header_value;
    settings->on_headers_complete = on_headers_complete;
    settings->on_body = on_body;
    settings->on_message_complete = on_message_complete;
    settings->on_status_complete = on_status_complete;
}

/*************************** uv callback functions ****************/

static void on_close_timer(uv_handle_t* handle) {
    uv_httpc_t* httpc = handle->data;
    free(httpc->timer);
    httpc->timer = NULL;
}

static void on_close(uv_handle_t* handle) {
    uv_httpc_t* httpc = handle->data;
    if (httpc->host)
        free(httpc->host);
    if (httpc->port)
        free(httpc->port);
    if (httpc->url)
        free(httpc->url);
    if (httpc->headers)
        uv_httpc_headers_free_all(httpc->headers, 1);
    if (httpc->http_data)
        free(httpc->http_data);
    if (httpc->addrinfo)
        uv_freeaddrinfo(httpc->addrinfo);

    mybuf_clear(&httpc->buf);
    reset_response(&httpc->response);
}

static void generic_on_response(uv_httpc_t* httpc, uv_httpc_code code, int uvcode) {
    if (code != 0) {
        if (httpc->timer) {
            uv_timer_stop(httpc->timer);
            uv_close((uv_handle_t*)httpc->timer, on_close_timer);
        }
        uv_close((uv_handle_t*)&httpc->tcp, on_close);
    }
    httpc->response.code = code;
    httpc->response.uvcode = uvcode;
    httpc->on_response(httpc, &httpc->response);
}

static void* memdup(const void* mem, size_t len) {
    void* p = malloc(len);
    fatal_if_null(p);
    memcpy(p, mem, len);
    return p;
}

static void on_alloc(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
    UNUSED(suggested_size);
    uv_httpc_t* httpc = handle->data;
    mybuf_reserve(&httpc->buf, DEFAULT_BUFF_SIZE);
    buf->base = httpc->buf.buf + httpc->buf.size;
#ifdef _WIN32
    buf->len = (ULONG)mybuf_space(&httpc->buf);
#else
    buf->len = mybuf_space(&httpc->buf);
#endif
}

static void on_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
    UNUSED(buf);
    uv_httpc_t* httpc = stream->data;
    enum llhttp_errno parse_ret;

    if (nread < 0) {
        generic_on_response(httpc, uv_httpc_connection_reset, (int)nread);
        return;
    } else if (nread == 0) {
        return;
    }

    parse_ret = llhttp_execute(&httpc->parser, httpc->buf.buf, nread);
    if (parse_ret != HPE_OK) {
        uvlog_error("uv_httpc llhttp_execute error: %s %s\n", llhttp_errno_name(parse_ret),
                    httpc->parser.reason);
        generic_on_response(httpc, uv_httpc_parse_failed, parse_ret);
    } else {
        // parse succeed, on_request_t should be called in on_message_complete
    }
    mybuf_clear(&httpc->buf);
}

static void on_write(uv_write_t* req, int status) {
    write_req_t* wr = (write_req_t*)req;
    mybuf_clear(&wr->buf);
    free(wr);

    if (status == 0) return;

    uvlog_error("uv_write error: %s - %s",
                uv_err_name(status),
                uv_strerror(status));
}

static int send_request(uv_httpc_t* httpc) {
    int r;
    write_req_t* req = malloc(sizeof *req);
    fatal_if_null(req);
    mybuf_init(&req->buf);
    mybuf_cat_printf(&req->buf, "%s %s HTTP/1.1\r\n", httpc->method, httpc->url);
    mybuf_cat_printf(&req->buf, "Host: %s\r\n", httpc->host);
    for (uv_httpc_headers_t* header = httpc->headers; header; header = header->next) {
        mybuf_cat_printf(&req->buf, "%s\r\n", header->data);
    }
    mybuf_cat_printf(&req->buf, "Content-length: %zu\r\n\r\n", httpc->len);
    mybuf_append(&req->buf, httpc->http_data, httpc->len);

    req->uvbuf.base = req->buf.buf;
#ifdef _WIN32
    req->uvbuf.len = (ULONG)req->buf.size;
#else
    req->uvbuf.len = req->buf.size;
#endif
    r = uv_write(&req->req, (uv_stream_t*)&httpc->tcp, &req->uvbuf, 1, on_write);
    if (r) {
        uvlog_error("uv_write failed: %s", uv_strerror(r));
        mybuf_clear(&req->buf);
        free(req);
        generic_on_response(httpc, uv_httpc_connection_reset, r);
    }
    return r;
}

static void on_connect(uv_connect_t* req, int status) {
    uv_httpc_t* httpc = req->data;
    if (status < 0) {
        warn_on_uv_err(status, "on_connect error");
        generic_on_response(httpc, uv_httpc_connection_failed, status);
    } else {
        mybuf_clear(&httpc->buf);
        setup_default_llhttp_settings(&httpc->http_settings);
        llhttp_init(&httpc->parser, HTTP_RESPONSE, &httpc->http_settings);
        httpc->parser.data = httpc;
        send_request(httpc);
        int r = uv_read_start((uv_stream_t*)&httpc->tcp, on_alloc, on_read);
        if (r) {
            warn_on_uv_err(r, "uv_read_start");
            generic_on_response(httpc, uv_httpc_connection_reset, r);
        }
    }
}

static void on_resolved(uv_getaddrinfo_t* resolver, int status, struct addrinfo* res) {
    int r;
    uv_httpc_t* httpc = resolver->data;

    if (status < 0) {
        uvlog_error("uv_httpc resovle '%s:%s' failed: %d %s\n",
                    httpc->host, httpc->port, status, uv_err_name(status));
        generic_on_response(httpc, uv_httpc_resolve_failed, status);
        return;
    }

    httpc->addrinfo = res;
    httpc->connect_req.data = httpc;
    r = uv_tcp_connect(&httpc->connect_req, &httpc->tcp,
                       (const struct sockaddr*)res->ai_addr, on_connect);
    fatal_on_uv_err(r, "uv_tcp_connect failed");
}

/*************************** public functions ****************/

int uv_httpc_create(uv_httpc_t** httpc, uv_loop_t* loop, const char* host, const char* port,
                    uv_httpc_on_response_t on_response) {
    if (!httpc || !loop || !host || !port || !on_response) return UV_EINVAL;
    uv_httpc_t* ptr = malloc(sizeof *ptr);
    if (!ptr) return UV_ENOMEM;
    int r = uv_tcp_init(loop, &ptr->tcp);
    if (r) {
        free(ptr);
        return r;
    }
    ptr->tcp.data = ptr;
    ptr->timer = NULL;
    ptr->data = NULL;
    ptr->stopped = 0;
    ptr->host = strdup(host);
    if (!ptr->host) { free(ptr); return UV_ENOMEM; }
    ptr->port = strdup(port);
    if (!ptr->port) { free(ptr->host); free(ptr); return UV_ENOMEM; }
    ptr->addrinfo = NULL;
    ptr->method = NULL;
    ptr->url = NULL;
    ptr->headers = NULL;
    ptr->http_data = NULL;
    ptr->len = 0;
    ptr->timeout = 0;  // 0 for forever
    ptr->on_response = on_response;
    mybuf_init(&ptr->buf);
    init_response(&ptr->response);

    *httpc = ptr;
    return 0;
}

int uv_httpc_post(uv_httpc_t* httpc, const char* url, uv_httpc_headers_t* headers,
                  const char* data, size_t len, int timeout) {
    if (!httpc || !url || (!data && len > 0)) return UV_EINVAL;
    int r = 0;
    httpc->method = "POST";
    if (!httpc->url || strcmp(httpc->url, url)) {
        if (httpc->url)
            free(httpc->url);
        httpc->url = strdup(url);
        if (!httpc->url) return UV_ENOMEM;
    }
    if (httpc->headers)
        uv_httpc_headers_free_all(httpc->headers, 1);
    httpc->headers = uv_httpc_headers_dup(headers);
    if (httpc->http_data)
        free(httpc->http_data);
    httpc->http_data = memdup(data, len);
    httpc->len = len;
    httpc->timeout = timeout;

    if (!httpc->addrinfo) {
        struct addrinfo hints;
        hints.ai_family = PF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        hints.ai_flags = 0;

        httpc->resolver.data = httpc;
        r = uv_getaddrinfo(httpc->tcp.loop, &httpc->resolver, on_resolved, httpc->host, httpc->port, &hints);
        fatal_on_uv_err(r, "uv_getaddrinfo failed");
    } else {
        send_request(httpc);
    }
    return r;
}

int uv_httpc_stop(uv_httpc_t* httpc) {
    if (!httpc || httpc->stopped) return 0;
    httpc->stopped = 1;
    if (httpc->timer) {
        uv_timer_stop(httpc->timer);
        uv_close((uv_handle_t*)httpc->timer, on_close_timer);
    }
    if (!uv_is_closing((uv_handle_t*)&httpc->tcp))
        uv_close((uv_handle_t*)&httpc->tcp, on_close);
    return 0;
}

void uv_httpc_free(uv_httpc_t* httpc) {
    free(httpc);
}

uv_httpc_headers_t* uv_httpc_headers_append_nodup(uv_httpc_headers_t* headers, char* data) {
    uv_httpc_headers_t* header = malloc(sizeof *header);
    if (!header) return NULL;
    header->next = NULL;
    header->data = data;
    if (!headers)
        return header;
    list_append(uv_httpc_headers_t, headers, header);
    return headers;
}

uv_httpc_headers_t* uv_httpc_headers_dup(uv_httpc_headers_t* headers) {
    uv_httpc_headers_t *dup = NULL, *tail = NULL;
    while (headers) {
        uv_httpc_headers_t* header = malloc(sizeof *header);
        fatal_if_null(header);
        header->data = strdup(headers->data);
        fatal_if_null(header->data);
        header->next = NULL;
        if (tail) {
            tail->next = header;
            tail = header;
        } else {
            dup = tail = header;
        }
        headers = headers->next;
    }
    return dup;
}

uv_httpc_headers_t* uv_httpc_headers_append(uv_httpc_headers_t* headers, char* data) {
    char* dup = strdup(data);
    if (!dup) return NULL;
    headers = uv_httpc_headers_append_nodup(headers, dup);
    if (!headers)
        free(dup);
    return headers;
}

void uv_httpc_headers_free_all(uv_httpc_headers_t* headers, int freedata) {
    while (headers) {
        uv_httpc_headers_t* next = headers->next;
        if (freedata)
            free(headers->data);
        free(headers);
        headers = next;
    }
}


