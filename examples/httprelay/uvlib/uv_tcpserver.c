#include "uv_tcpserver.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "uv_log.h"

#define DEFAULT_BUFF_SIZE 1024

typedef struct {
    uv_write_t req;
    uv_buf_t buf;
} write_req_t;

static void on_close_server(uv_handle_t* handle) {
    uv_tcpserver_t* server = handle->data;
    if (server->settings->on_server_shutdown)
        server->settings->on_server_shutdown(server);
}

static void on_close(uv_handle_t* stream) {
    uv_tcpserver_client_context_t* cli = stream->data;
    if (cli->server->settings->on_connect)
        cli->server->settings->on_connect(cli, 0);
    else
        uv_tcpserver_free_cli_ctx(cli);
}

static void on_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
    uv_tcpserver_client_context_t* cli = stream->data;
    assert(stream == (uv_stream_t*)&cli->tcp);
    if (nread < 0) {
        uv_close((uv_handle_t*)stream, on_close);
        return;
    } else if (nread == 0) {
        return;
    }

    if (cli->server->settings->on_parse) {
        cli->buf.size += nread;
        size_t cb = 0, total = cli->buf.size;
        const char* p = cli->buf.buf;
        uv_tcp_parse_result_t r = cli->server->settings->on_parse(cli, p, total, &cb);
        while (r == uv_tcp_parse_ok && total > 0) {
            p += cb;
            total -= cb;
            if (total > 0)
                r = cli->server->settings->on_parse(cli, p, total, &cb);
        }
        if (r == uv_tcp_parse_error) {
            p += cb;
            total -= cb;
        }
        if (p != cli->buf.buf) {
            if (total > 0) {
                mybuf_clear_append(&cli->buf, p, total);
            } else {
                mybuf_clear(&cli->buf);
            }
        }
    } else {
        mybuf_clear(&cli->buf);
    }
}

static void on_alloc(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
    uv_tcpserver_client_context_t* cli = handle->data;
    mybuf_reserve(&cli->buf, DEFAULT_BUFF_SIZE);
    buf->base = cli->buf.buf + cli->buf.size;
#ifdef _WIN32
    buf->len = (ULONG)mybuf_space(&cli->buf);
#else
    buf->len = mybuf_space(&cli->buf);
#endif
}

static void on_connection(uv_stream_t* server, int status) {
    int r = 0;
    uv_tcpserver_client_context_t* client = NULL;
    uv_tcpserver_t* server_ctx = server->data;

    if (status != 0) {
        warn_on_uv_err(status, "uv_tcpserver.on_connection");
        return;
    }

    client = malloc(sizeof *client);
    fatal_if_null(client);

    do {
        r = uv_tcp_init(server->loop, &client->tcp);
        if (r) break;
        r = uv_accept(server, (uv_stream_t*)&client->tcp);
        if (r) break;
        r = uv_read_start((uv_stream_t*)&client->tcp, on_alloc, on_read);
        if (r) break;
        client->tcp.data = client;
        client->server = server_ctx;
        client->data = NULL;
        mybuf_init(&client->buf);
        if (server_ctx->settings->on_connect)
            server_ctx->settings->on_connect(client, 1);
    } while (0);

    if (r) {
        if (client)
            free(client);
    }
}

int uv_tcpserver_create(uv_tcpserver_t** server, uv_loop_t* loop, uv_tcpserver_settings_t* settings) {
    int r;
    uv_tcpserver_t* ptr = malloc(sizeof *ptr);
    if (!ptr) return UV_ENOMEM;
    r = uv_tcp_init(loop, &ptr->tcp);
    if (r) {
        free(ptr);
        return r;
    }
    ptr->tcp.data = ptr;
    ptr->settings = settings;
    *server = ptr;
    return 0;
}

int uv_tcpserver_start_listen(uv_tcpserver_t* server, const char* listen_addr, int listen_port) {
    struct sockaddr_in addr;
    int r = uv_ip4_addr(listen_addr, listen_port, &addr);
    if (r) return r;
    r = uv_tcp_bind(&server->tcp, (const struct sockaddr*)&addr, 0);
    if (r) return r;
    r = uv_listen((uv_stream_t*)&server->tcp, SOMAXCONN, on_connection);
    return r;
}

void uv_tcpserver_stop(uv_tcpserver_t* server) {
    uv_close((uv_handle_t*)&server->tcp, on_close_server);
}

void uv_tcpserver_free(uv_tcpserver_t* server) {
    free(server);
}

void uv_tcpserver_close_cli(uv_tcpserver_client_context_t* cli) {
    uv_close((uv_handle_t*)&cli->tcp, on_close);
}

void uv_tcpserver_free_cli_ctx(uv_tcpserver_client_context_t* cli) {
    mybuf_clear(&cli->buf);
    free(cli);
}

static void after_write(uv_write_t* req, int status) {
    write_req_t* wr = (write_req_t*)req;
    free(wr->buf.base);
    free(wr);
    if (status == 0) return;
    uvlog_error("uv_write error: %s - %s\n", uv_err_name(status), uv_strerror(status));
}

int uv_tcpserver_send_to_cli(uv_tcpserver_client_context_t* cli, const char* buf, size_t len) {
    int r;
    write_req_t* req;
    req = malloc(sizeof *req);
    if (!req) return UV_ENOMEM;
    req->buf.base = malloc(len);
    if (!req->buf.base) {
        free(req);
        return UV_ENOMEM;
    }
#ifdef _WIN32
    req->buf.len = (ULONG)len;
#else
    req->buf.len = len;
#endif
    memcpy(req->buf.base, buf, len);
    r = uv_write(&req->req, (uv_stream_t*)&cli->tcp, &req->buf, 1, after_write);
    if (r) {
        free(req->buf.base);
        free(req);
    }
    return r;
}
