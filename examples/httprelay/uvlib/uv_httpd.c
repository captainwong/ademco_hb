#include "uv_httpd.h"

#include <assert.h>
#include <ctype.h>
#include <libemb/emb_string.h>
#include <libemb/libemb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mybuf.h"
#include "uv_log.h"
#include "uv_tcp.h"

static int enable_print = 0;

#define dprintf \
    if (enable_print) printf
#define dnprintf \
    if (enable_print) nprintf
#define print_func dprintf("%s\n", __func__)

#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif

// uv_httpd_client_s is now defined in uv_httpd.h (with linked-list support)
// keep local accessors for buf/pkt as mybuf_t*
#define CLIENT_BUF(client)  ((mybuf_t*)(client)->buf)
#define CLIENT_PKT(client)  ((mybuf_t*)(client)->pkt)
#define CLIENT_HEADERS(client) ((uv_httpd_header_t*)(client)->headers)

struct write_req_t {
    uv_write_t req;
    uv_buf_t buf;
};

static void on_close(uv_handle_t* peer);

/*************************** helper functions ****************/

static void reset_request(uv_httpd_client_t* client) {
    if (client->req.headers.headers != CLIENT_HEADERS(client)) {
        free(client->req.headers.headers);
        client->req.headers.headers = CLIENT_HEADERS(client);
    }
    client->req.headers.n = 0;
    client->req.base = NULL;
    client->req.method = 0;
    client->req.url.offset = 0;
    client->req.url.len = 0;
    client->req.version.offset = 0;
    client->req.version.len = 0;
    client->req.body.offset = 0;
    client->req.body.len = 0;
}

static void headers_append_key(uv_httpd_client_t* client, size_t offset, size_t len) {
    if (client->req.headers.n == 16) {
        uv_httpd_header_t* headers = malloc((client->req.headers.n + 1) * sizeof(uv_httpd_header_t));
        fatal_if_null(headers);
        memcpy(headers, client->req.headers.headers, (client->req.headers.n) * sizeof(uv_httpd_header_t));
        client->req.headers.headers = headers;
    } else if (client->req.headers.n > 16) {
        uv_httpd_header_t* headers = realloc(client->req.headers.headers, (client->req.headers.n + 1) * sizeof(uv_httpd_header_t));
        fatal_if_null(headers);
        client->req.headers.headers = headers;
    }
    client->req.headers.headers[client->req.headers.n].key.offset = offset;
    client->req.headers.headers[client->req.headers.n].key.len = len;
}

static void headers_append_value(uv_httpd_client_t* client, size_t offset, size_t len) {
    client->req.headers.headers[client->req.headers.n].value.offset = offset;
    client->req.headers.headers[client->req.headers.n].value.len = len;
    client->req.headers.n++;
}

/*************************** llhttp callback functions ****************/

static int on_message_begin(llhttp_t* llhttp) {
    print_func;
    uv_httpd_client_t* client = llhttp->data;
    mybuf_clear(CLIENT_PKT(client));
    reset_request(client);
    return 0;
}

static int on_url(llhttp_t* llhttp, const char* at, size_t length) {
    print_func;
    dnprintf(at, length, 1);
    uv_httpd_client_t* client = llhttp->data;
    client->req.url.offset = CLIENT_PKT(client)->size;
    client->req.url.len = length;
    mybuf_append(CLIENT_PKT(client), at, length);
    return 0;
}

static int on_status(llhttp_t* llhttp, const char* at, size_t length) {
    UNUSED(llhttp);
    print_func;
    dnprintf(at, length, 1);
    return 0;
}

static int on_method(llhttp_t* llhttp, const char* at, size_t length) {
    UNUSED(llhttp);
    print_func;
    dnprintf(at, length, 1);
    return 0;
}

static int on_version(llhttp_t* llhttp, const char* at, size_t length) {
    print_func;
    dnprintf(at, length, 1);
    uv_httpd_client_t* client = llhttp->data;
    client->req.version.offset = CLIENT_PKT(client)->size;
    client->req.version.len = length;
    mybuf_append(CLIENT_PKT(client), at, length);
    return 0;
}

static int on_header_field(llhttp_t* llhttp, const char* at, size_t length) {
    print_func;
    dnprintf(at, length, 1);
    uv_httpd_client_t* client = llhttp->data;
    headers_append_key(client, CLIENT_PKT(client)->size, length);
    mybuf_append(CLIENT_PKT(client), at, length);
    return 0;
}

static int on_header_value(llhttp_t* llhttp, const char* at, size_t length) {
    print_func;
    dnprintf(at, length, 1);
    uv_httpd_client_t* client = llhttp->data;
    headers_append_value(client, CLIENT_PKT(client)->size, length);
    mybuf_append(CLIENT_PKT(client), at, length);
    return 0;
}

static int on_chunk_extension_name(llhttp_t* llhttp, const char* at, size_t length) {
    UNUSED(llhttp);
    print_func;
    dnprintf(at, length, 1);
    return 0;
}

static int on_chunk_extension_value(llhttp_t* llhttp, const char* at, size_t length) {
    UNUSED(llhttp);
    print_func;
    dnprintf(at, length, 1);
    return 0;
}

static int on_headers_complete(llhttp_t* llhttp) {
    UNUSED(llhttp);
    print_func;
    return 0;
}

static int on_body(llhttp_t* llhttp, const char* at, size_t length) {
    print_func;
    dnprintf(at, length, 1);
    uv_httpd_client_t* client = llhttp->data;
    if (client->req.body.len + length > UV_HTTPD_MAX_BODY_SIZE) {
        uvlog_warn("Request body too large (%zu + %zu)", client->req.body.len, length);
        return -1;  // 返回非零值会停止解析
    }
    /* 只在第一块 body 数据到来时记录起始偏移 */
    if (client->req.body.len == 0) {
        client->req.body.offset = CLIENT_PKT(client)->size;
    }
    /* 累加总长度，而非每次覆盖 */
    client->req.body.len += length;
    mybuf_append(CLIENT_PKT(client), at, length);
    return 0;
}

/* Drain pending writes before FIN: uv_close directly would cancel
 * queued writes and truncate large responses mid-flight. */
static void on_shutdown(uv_shutdown_t* req, int status) {
    uv_stream_t* stream = req->handle;
    free(req);
    if (status) {
        uvlog_debug("httpd shutdown failed: %d: %s", status, uv_strerror(status));
    }
    uv_close((uv_handle_t*)stream, on_close);
}

static int on_message_complete(llhttp_t* llhttp) {
    print_func;
    uv_httpd_client_t* client = llhttp->data;
    client->req.base = CLIENT_PKT(client)->buf;
    client->on_request(client->server, client, &client->req);
    /* HTTP/1.1 defaults to keep-alive; only explicit "Connection: close"
     * (or HTTP/1.0 without keep-alive) should end the connection. */
    if (!llhttp_should_keep_alive(&client->parser) && !uv_is_closing((uv_handle_t*)&client->tcp)) {
        uv_shutdown_t* req = malloc(sizeof *req);
        if (!req || uv_shutdown(req, (uv_stream_t*)&client->tcp, on_shutdown) != 0) {
            free(req);
            uv_close((uv_handle_t*)&client->tcp, on_close);
        }
    }
    reset_request(client);
    return 0;
}

static int on_url_complete(llhttp_t* llhttp) {
    UNUSED(llhttp);
    print_func;
    return 0;
}

static int on_status_complete(llhttp_t* llhttp) {
    UNUSED(llhttp);
    print_func;
    return 0;
}

static int on_method_complete(llhttp_t* llhttp) {
    print_func;
    uv_httpd_client_t* client = llhttp->data;
    client->req.method = llhttp->method;
    return 0;
}

static int on_version_complete(llhttp_t* llhttp) {
    UNUSED(llhttp);
    print_func;
    return 0;
}

static int on_header_field_complete(llhttp_t* llhttp) {
    UNUSED(llhttp);
    print_func;
    return 0;
}

static int on_header_value_complete(llhttp_t* llhttp) {
    UNUSED(llhttp);
    print_func;
    return 0;
}

static int on_chunk_extension_name_complete(llhttp_t* llhttp) {
    UNUSED(llhttp);
    print_func;
    return 0;
}

// static int on_chunk_extension_value_complete(llhttp_t* llhttp) {
//	print_func;
//	return 0;
// }

static int on_chunk_header(llhttp_t* llhttp) {
    UNUSED(llhttp);
    print_func;
    return 0;
}

static int on_chunk_complete(llhttp_t* llhttp) {
    UNUSED(llhttp);
    print_func;
    return 0;
}

static int on_reset(llhttp_t* llhttp) {
    UNUSED(llhttp);
    print_func;
    return 0;
}

static void setup_default_llhttp_settings(llhttp_settings_t* settings) {
    llhttp_settings_init(settings);  // 先归零
    settings->on_message_begin = on_message_begin;
    settings->on_url = on_url;
    settings->on_status = on_status;
    settings->on_method = on_method;
    settings->on_version = on_version;
    settings->on_header_field = on_header_field;
    settings->on_header_value = on_header_value;
    settings->on_chunk_extension_name = on_chunk_extension_name;
    settings->on_chunk_extension_value = on_chunk_extension_value;
    settings->on_headers_complete = on_headers_complete;
    settings->on_body = on_body;
    settings->on_message_complete = on_message_complete;
    settings->on_url_complete = on_url_complete;
    settings->on_status_complete = on_status_complete;
    settings->on_method_complete = on_method_complete;
    settings->on_version_complete = on_version_complete;
    settings->on_header_field_complete = on_header_field_complete;
    settings->on_header_value_complete = on_header_value_complete;
    settings->on_chunk_extension_name_complete = on_chunk_extension_name_complete;
    settings->on_chunk_header = on_chunk_header;
    settings->on_chunk_complete = on_chunk_complete;
    settings->on_reset = on_reset;
}

/*************************** uv callback functions ****************/

static void on_write(uv_write_t* req, int status) {
    print_func;
    if (status) {
        // client disconnected mid-write (ECONNRESET/EPIPE/ECANCELED), normal event
        uvlog_debug("httpd write failed: %d: %s", status, uv_strerror(status));
    }
    struct write_req_t* wr = req->data;
    free(wr->buf.base);
    free(wr);
}

static void on_alloc(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
    print_func;
    dprintf("suggested_size=%zu\n", suggested_size);
    uv_httpd_client_t* client = handle->data;
    mybuf_reserve(CLIENT_BUF(client), suggested_size);
    buf->base = CLIENT_BUF(client)->buf;
#ifdef _WIN32
    buf->len = (ULONG)mybuf_space(CLIENT_BUF(client));
#else
    buf->len = mybuf_space(CLIENT_BUF(client));
#endif
}

static void on_close(uv_handle_t* peer) {
    print_func;
    uv_httpd_client_t* client = peer->data;

    // remove from server's client linked list
    if (client->server) {
        uv_httpd_server_t* server = client->server;
        uv_httpd_client_t** pp = &server->clients;
        while (*pp) {
            if (*pp == client) {
                *pp = client->next;
                server->nclients--;
                break;
            }
            pp = &(*pp)->next;
        }
    }

    if (client->user_on_closing.on_closing) {
        client->user_on_closing.on_closing(client, client->user_on_closing.user);
    }
    mybuf_clear(CLIENT_BUF(client));
    mybuf_clear(CLIENT_PKT(client));
    reset_request(client);
    free(CLIENT_BUF(client));  // free heap-allocated mybuf_t
    free(CLIENT_PKT(client));  // free heap-allocated mybuf_t
    free(peer);  // since our uv_tcpclient_t's first member is uv_tcp_t, so peer's addr IS our client's addr, just free it.
}

static void on_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
    print_func;
    dprintf("nread= %zd\n", nread);
    uv_httpd_client_t* client = stream->data;
    enum llhttp_errno parse_ret;

    if (nread < 0) {
        uv_close((uv_handle_t*)stream, on_close);
        return;
    } else if (nread == 0) {
        return;
    }

    dprintf("before llhttp_execute\n");
    // dnprintf(buf->base, nread, 1);
    if (enable_print) {
        uv_log_hexdump(uv_log_level_debug, buf->base, nread, 1, 1, 1, '.', "\n");
    }
    parse_ret = llhttp_execute(&client->parser, buf->base, nread);
    if (parse_ret != HPE_OK) {
        uvlog_error("Parse error: %s %s\n", llhttp_errno_name(parse_ret),
                    client->parser.reason);
        uv_close((uv_handle_t*)stream, on_close);
    } else {
        // parse succeed, on_request_t should be called in on_message_complete
    }
    dprintf("after llhttp_execute\n");
    mybuf_clear(CLIENT_BUF(client));
}

static void on_connected(uv_stream_t* stream, int status) {
    print_func;
    if (status) {
        uvlog_debug("httpd accept failed: %d: %s", status, uv_strerror(status));
        return;
    }
    uv_httpd_server_t* server = stream->data;
    uv_httpd_client_t* client = calloc(1, sizeof *client);
    if (!client) {
        warn_on_uv_err(UV_ENOMEM, "calloc client");
        return;
    }
    int r = uv_tcp_init(stream->loop, &client->tcp);
    if (r) {
        warn_on_uv_err(r, "uv_tcp_init failed");
        free(client);
        return;
    }
    r = uv_accept(stream, (uv_stream_t*)&client->tcp);
    if (r) {
        warn_on_uv_err(r, "uv_accept error");
        uv_close((uv_handle_t*)&client->tcp, on_close);
        return;
    }

    // allocate mybuf_t on heap
    mybuf_t* buf = malloc(sizeof(mybuf_t));
    mybuf_t* pkt = malloc(sizeof(mybuf_t));
    if (!buf || !pkt) {
        free(buf);
        free(pkt);
        warn_on_uv_err(UV_ENOMEM, "malloc mybuf_t");
        uv_close((uv_handle_t*)&client->tcp, on_close);
        return;
    }
    mybuf_init(buf);
    mybuf_init(pkt);

    client->server = server;
    client->on_request = server->on_request;
    client->tcp.data = client;
    client->buf = buf;
    client->pkt = pkt;
    llhttp_init(&client->parser, HTTP_REQUEST, &server->http_settings);
    client->parser.data = client;
    client->req.headers.headers = CLIENT_HEADERS(client);
    client->req.headers.n = 0;
    client->user_on_closing.user = NULL;
    client->user_on_closing.on_closing = NULL;
    client->next = NULL;
    getpeeraddr(&client->tcp, client->req.ip, sizeof(client->req.ip), NULL);

    // add to server's client linked list
    client->next = server->clients;
    server->clients = client;
    server->nclients++;

    r = uv_read_start((uv_stream_t*)&client->tcp, on_alloc, on_read);
    if (r) {
        warn_on_uv_err(r, "uv_read_start");
        uv_close((uv_handle_t*)&client->tcp, on_close);
    }
}

/*************************** public functions ****************/

void uv_httpd_enable_printf(int enable) {
    enable_print = enable;
}

int uv_httpd_create(uv_httpd_server_t** server, uv_loop_t* loop, on_request_t on_request) {
    if (!server || !loop || !on_request) return UV_EINVAL;
    int r = UV_ENOMEM;
    uv_httpd_server_t* s;

    s = calloc(1, sizeof(*s));
    if (!s) {
        goto failed;
    }

    r = uv_tcp_init(loop, &s->tcp);
    if (r) {
        goto failed;
    }
    s->tcp.data = s;
    s->on_request = on_request;
    setup_default_llhttp_settings(&s->http_settings);

    *server = s;
    return 0;

failed:
    uv_httpd_free(s);
    return r;
}

static void on_server_closed(uv_handle_t* handle) {
    UNUSED(handle);
    uvlog_debug("uv_httpd.tcp closed");
}

void uv_httpd_stop(uv_httpd_server_t* server) {
    // close all active clients first
    uv_httpd_client_t* client = server->clients;
    while (client) {
        uv_httpd_client_t* next = client->next;
        if (!uv_is_closing((uv_handle_t*)&client->tcp)) {
            uv_close((uv_handle_t*)&client->tcp, on_close);
        }
        client = next;
    }
    // then close the listen socket
    if (!uv_is_closing((uv_handle_t*)&server->tcp))
        uv_close((uv_handle_t*)&server->tcp, on_server_closed);
}

void uv_httpd_free(uv_httpd_server_t* server) {
    free(server);
}

int uv_httpd_listen(uv_httpd_server_t* server, const char* ip, int port) {
    int r;
    struct sockaddr_in addr;

    r = uv_ip4_addr(ip, port, &addr);
    if (r) return r;

    r = uv_tcp_bind(&server->tcp, (const struct sockaddr*)&addr, 0);
    if (r) return r;

    r = uv_listen((uv_stream_t*)&server->tcp, SOMAXCONN, on_connected);
    if (r) return r;

    return r;
}

int uv_httpd_write_response(uv_httpd_client_t* client, char* response, size_t len) {
    if (!client || !response) return UV_EINVAL;
    struct write_req_t* req = malloc(sizeof *req);
    if (!req) return UV_ENOMEM;
    req->buf.base = malloc(len);
    if (!req->buf.base) {
        free(req);
        return UV_ENOMEM;
    }
    memcpy(req->buf.base, response, len);
#ifdef _WIN32
    req->buf.len = (ULONG)len;
#else
    req->buf.len = len;
#endif
    req->req.data = req;
    int r = uv_write(&req->req, (uv_stream_t*)&client->tcp, &req->buf, 1, on_write);
    if (r) {
        free(req->buf.base);
        free(req);
    }
    return r;
}

void uv_httpd_setup_on_client_closing(uv_httpd_client_t* client, on_httpd_client_closing_cb_t on_closing, void* user) {
    client->user_on_closing.on_closing = on_closing;
    client->user_on_closing.user = user;
}
