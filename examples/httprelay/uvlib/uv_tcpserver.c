#include "uv_tcpserver.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "uv_log.h"

#define DEFAULT_BUFF_SIZE 1024

#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif

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
    uv_tcpserver_free_cli_ctx(cli);
}

// Remove client from server's pending list (if present).
static void pending_remove(uv_tcpserver_t* server, uv_tcpserver_client_context_t* cli) {
    uv_tcpserver_client_context_t** prev = &server->pending_head;
    while (*prev) {
        if (*prev == cli) {
            *prev = cli->next;
            cli->next = NULL;
            return;
        }
        prev = &(*prev)->next;
    }
}

static void on_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
    UNUSED(buf);
    uv_tcpserver_client_context_t* cli = stream->data;
    assert(stream == (uv_stream_t*)&cli->tcp);
    if (nread < 0) {
        // Remove from pending list before notifying upper layer
        pending_remove(cli->server, cli);
        if (cli->server->settings->on_connect)
            cli->server->settings->on_connect(cli, 0);
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
        while ((r == uv_tcp_parse_ok || r == uv_tcp_parse_error) && total > 0 && cb > 0) {
            p += cb;
            total -= cb;
            if (total > 0)
                r = cli->server->settings->on_parse(cli, p, total, &cb);
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
    size_t sz = suggested_size > 0 ? suggested_size : DEFAULT_BUFF_SIZE;
    uv_tcpserver_client_context_t* cli = handle->data;
    mybuf_reserve(&cli->buf, sz);
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

    client = calloc(1, sizeof(*client));
    fatal_if_null(client);

    /* Initialize fields before starting any I/O operations */
    client->server = server_ctx;
    client->data = NULL;
    mybuf_init(&client->buf);
    client->tcp.data = client;

    r = uv_tcp_init(server->loop, &client->tcp);
    if (r) {
        free(client);
        return;
    }

    r = uv_accept(server, (uv_stream_t*)&client->tcp);
    if (r) goto error;

    r = uv_read_start((uv_stream_t*)&client->tcp, on_alloc, on_read);
    if (r) goto error;

    if (server_ctx->settings->on_connect) {
        int taken = server_ctx->settings->on_connect(client, 1);
        if (!taken) {
            // Upper layer has NOT taken ownership, add to pending list
            client->next = server_ctx->pending_head;
            server_ctx->pending_head = client;
        }
    } else {
        // No callback, add to pending list by default
        client->next = server_ctx->pending_head;
        server_ctx->pending_head = client;
    }
    return;

error:
    /* uv_close triggers on_close -> uv_tcpserver_free_cli_ctx -> free(client) */
    uv_close((uv_handle_t*)&client->tcp, on_close);
}

int uv_tcpserver_create(uv_tcpserver_t** server, uv_loop_t* loop, uv_tcpserver_settings_t* settings) {
    int r;
    if (!server || !loop || !settings) return UV_EINVAL;
    uv_tcpserver_t* ptr = calloc(1, sizeof(*ptr));
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
    // Close all pending (not-yet-taken) clients first.
    // uv_tcpserver_close_cli triggers on_connect(client, 0) so upper layer can clean up.
    uv_tcpserver_client_context_t* cli = server->pending_head;
    while (cli) {
        uv_tcpserver_client_context_t* next = cli->next;
        uv_tcpserver_close_cli(cli);
        cli = next;
    }
    server->pending_head = NULL;

    // Then close the listen socket
    if (!uv_is_closing((uv_handle_t*)&server->tcp))
        uv_close((uv_handle_t*)&server->tcp, on_close_server);
}

/**
 * Free the server. Must be called only after uv_tcpserver_stop() has been called
 * and the server shutdown callback has been received.
 */
void uv_tcpserver_free(uv_tcpserver_t* server) {
    free(server);
}

void uv_tcpserver_close_cli(uv_tcpserver_client_context_t* cli) {
    // Remove from pending list first (if still there)
    pending_remove(cli->server, cli);
    // Notify upper layer so it can clean up cli->data
    if (cli->server->settings->on_connect)
        cli->server->settings->on_connect(cli, 0);
    uv_close((uv_handle_t*)&cli->tcp, on_close);
}

void uv_tcpserver_take_cli(uv_tcpserver_client_context_t* cli) {
    pending_remove(cli->server, cli);
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
    uvlog_error("uv_write error: %s - %s", uv_err_name(status), uv_strerror(status));
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
