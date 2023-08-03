#include <assert.h>
#include <string.h>
#include "uv_tcpclient.h"
#include "uv_log.h"
#include "mybuf.h"

#define DEFAULT_BUFF_SIZE 1024

typedef struct {
	uv_write_t req;
	uv_buf_t buf;
}write_req_t;

static void client_init(uv_tcpclient_t* cli) {
	cli->data = NULL;
	cli->loop = NULL;
	cli->tcp = NULL;
	cli->reconn_timer = NULL;
	cli->enable_reconn = 0;
	cli->settings = NULL;
	cli->domain = NULL;
	cli->port = NULL;
	mybuf_init(&cli->buf);
	cli->connected = 0;
}

/********************************** uv callbacks ****************************/

static void on_write(uv_write_t* req, int status) {
	write_req_t* wr = (write_req_t*)req;
	free(wr->buf.base);
	free(wr);

	if (status == 0) return;

	uvlog_error("uv_write error: %s - %s\n",
				uv_err_name(status),
				uv_strerror(status));
}

static void on_close_reconn_timer(uv_handle_t* timer) {
	uv_tcpclient_t* cli = timer->data;
	assert(timer == (uv_handle_t*)cli->reconn_timer);
	free(cli->reconn_timer);
	cli->reconn_timer = NULL;
}

static void on_reconn(uv_timer_t* timer) {
	uv_tcpclient_t* cli = timer->data;
	assert(timer == cli->reconn_timer);
	uv_tcpclient_start(cli, cli->domain, cli->port);
	uv_close((uv_handle_t*)timer, on_close_reconn_timer);
}

static void on_close(uv_handle_t* stream) {
	int r;
	uv_tcpclient_t* cli = stream->data;
	if (cli->tcp) {
		free(cli->tcp);
		cli->tcp = NULL;
	}
	if (cli->settings->on_connect)
		cli->settings->on_connect(cli, 0);
	cli->connected = 0;

	if (cli->enable_reconn) {
		// start reconnect
		cli->reconn_timer = malloc(sizeof(uv_timer_t));
		fatal_if_null(cli->reconn_timer);
		r = uv_timer_init(cli->loop, cli->reconn_timer);
		fatal_on_uv_err(r, "uv_timer_init failed");
		cli->reconn_timer->data = cli;
		uv_timer_start(cli->reconn_timer, on_reconn, 3000, 0);
	} else if (cli->settings->on_close) {
		cli->settings->on_close(cli);
	}
}

static void on_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
	uv_tcpclient_t* cli = stream->data;
	assert(stream == (uv_stream_t*)cli->tcp);
	if (nread < 0) {
		uv_close((uv_handle_t*)stream, on_close);
		return;
	} else if (nread == 0) {
		return;
	}

	if (cli->settings->on_parse) {
		cli->buf.size += nread;
		size_t cb, total = cli->buf.size;
		const char* p = cli->buf.buf;
		uv_tcp_parse_result_t r = cli->settings->on_parse(cli, p, total, &cb);
		while (r == uv_tcp_parse_ok && total > 0) {
			p += cb;
			total -= cb;
			if (total > 0)
				r = cli->settings->on_parse(cli, p, total, &cb);
		}
		if (r == uv_tcp_parse_error) {
			mybuf_clear(&cli->buf);
		} else if (p != cli->buf.buf) {
			if (total > 0) {
				mybuf_t buf;
				mybuf_init(&buf);
				mybuf_append(&buf, p, total);
				mybuf_clear(&cli->buf);
				mybuf_append(&cli->buf, buf.buf, buf.size);
			} else {
				mybuf_clear(&cli->buf);
			}
		}
	} else {
		mybuf_clear(&cli->buf);
	}
}

static void on_alloc(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
	uv_tcpclient_t* cli = handle->data;
	mybuf_reserve(&cli->buf, DEFAULT_BUFF_SIZE);
	buf->base = cli->buf.buf + cli->buf.size;
#ifdef _WIN32
	buf->len = (ULONG)mybuf_space(&cli->buf);
#else
	buf->len = mybuf_space(&cli->buf);
#endif
}

static void on_connect(uv_connect_t* req, int status) {
	uv_tcpclient_t* cli = req->data;
	if (status < 0) {
		warn_on_uv_err(status, "on_connect error");
		if (!uv_is_closing((uv_handle_t*)cli->tcp))
			uv_close((uv_handle_t*)cli->tcp, on_close);
	} else {
		cli->connected = 1;
		mybuf_clear(&cli->buf);
		uv_read_start((uv_stream_t*)cli->tcp, on_alloc, on_read);
		if (cli->settings->on_connect) {
			cli->settings->on_connect(cli, 1);
		}
	}
}

static void on_resolved(uv_getaddrinfo_t* resolver, int status, struct addrinfo* res) {
	int r;
	uv_tcpclient_t* cli = resolver->data;

	if (status < 0) {
		uvlog_fatal("client resovle '%s:%s' failed: %d %s\n",
					cli->domain, cli->port, status, uv_err_name(status));
		exit(status);
	}

	cli->tcp = malloc(sizeof(uv_tcp_t));
	fatal_if_null(cli->tcp);
	
	r = uv_tcp_init(cli->loop, cli->tcp);
	fatal_on_uv_err(r, "uv_tcp_init failed");
	
	cli->tcp->data = cli;
	cli->connect_req.data = cli;
	r = uv_tcp_connect(&cli->connect_req, cli->tcp,
					   (const struct sockaddr*)res->ai_addr, on_connect);
	fatal_on_uv_err(r, "uv_tcp_connect failed");
	uv_freeaddrinfo(res);
}


/********************************** uv_tcpclient_t functions ****************************/

int uv_tcpclient_new(uv_tcpclient_t** client, uv_loop_t* loop, uv_tcpclient_settings_t* settings) {
	uv_tcpclient_t* cli = malloc(sizeof(*cli));
	if (!cli) return UV_ENOMEM;
	client_init(cli);
	cli->loop = loop;
	if (settings)
		cli->settings = settings;
	*client = cli;
	return 0;
}

int uv_tcpclient_start(uv_tcpclient_t* cli, const char* domain, const char* port) {
	int r;
	struct addrinfo hints;

	if (domain != cli->domain) {
		if (cli->domain)
			free(cli->domain);
		cli->domain = strdup(domain);
	}
	if (port != cli->port) {
		if (cli->port)
			free(cli->port);
		cli->port = strdup(port);
	}
	hints.ai_family = PF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = 0;

	cli->resolver.data = cli;
	r = uv_getaddrinfo(cli->loop, &cli->resolver, on_resolved, domain, port, &hints);
	fatal_on_uv_err(r, "uv_getaddrinfo failed");
	return r;
}

void uv_tcpclient_send(uv_tcpclient_t* client, const char* buf, size_t len) {
	if (!client->connected) return;
	write_req_t* req = malloc(sizeof * req);
	fatal_if_null(req);
	req->buf.base = malloc(len);
	fatal_if_null(req->buf.base);
	memcpy(req->buf.base, buf, len);
#ifdef _WIN32
	req->buf.len = (ULONG)len;
#else
	req->buf.len = len;
#endif
	uv_write(&req->req, (uv_stream_t*)client->tcp, &req->buf, 1, on_write);
}

void uv_tcpclient_stop(uv_tcpclient_t* client) {
	if (client->tcp && !uv_is_closing((uv_handle_t*)client->tcp)) {
		uv_close((uv_handle_t*)client->tcp, on_close);
	}
}

void uv_tcpclient_free(uv_tcpclient_t* client) {
	if (client->domain)
		free(client->domain);
	if (client->port)
		free(client->port);
	free(client);
}
