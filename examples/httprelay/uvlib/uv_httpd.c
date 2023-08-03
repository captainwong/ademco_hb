#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "uv_httpd.h"
#include "mybuf.h"
#include "uv_log.h"

static int enable_print = 0;

#define dprintf if (enable_print) printf
#define dnprintf if (enable_print) nprintf
#define print_func dprintf("%s\n", __FUNCTION__);


#define HEADERS_DEFAULT_LENGTH 16
#define DEFAULT_BUFF_SIZE 1024


struct uv_httpd_client_s {
	uv_tcp_t tcp;
	uv_httpd_server_t* server;
	llhttp_t parser;
	on_request_t on_request;
	uv_httpd_request_t req;
	uv_httpd_header_t headers[HEADERS_DEFAULT_LENGTH];
	mybuf_t buf;
	mybuf_t pkt;
};

struct write_req_t {
	uv_write_t req;
	uv_buf_t buf;
};

static void on_close(uv_handle_t* peer);


/*************************** helper functions ****************/

static void reset_request(uv_httpd_client_t* client) {
	if (client->req.headers.headers != client->headers) {
		free(client->req.headers.headers);
	}
	memset((char*)&client->req + sizeof(client->req.ip), 0, sizeof(uv_httpd_request_t) - sizeof(client->req.ip));
	client->req.headers.headers = client->headers;
}

static void headers_append_key(uv_httpd_client_t* client, size_t offset, size_t len) {
	if (client->req.headers.n == HEADERS_DEFAULT_LENGTH) {
		uv_httpd_header_t* headers = malloc((client->req.headers.n + 1) * sizeof(uv_httpd_header_t));
		fatal_if_null(headers);
		memcpy(headers, client->req.headers.headers, (client->req.headers.n) * sizeof(uv_httpd_header_t));
		client->req.headers.headers = headers;
	} else if (client->req.headers.n > HEADERS_DEFAULT_LENGTH) {
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

static int headers_contains(uv_httpd_client_t* client, const char* key, const char* value) {
	for (size_t i = 0; i < client->req.headers.n; i++) {
		uv_httpd_header_t header = client->req.headers.headers[i];
		if(0 == string0_nicmp(key, client->req.base + header.key.offset, header.key.len)
		   && 0 == string0_nicmp(value, client->req.base + header.value.offset, header.value.len)) {
			return 1;
		}
	}
	return 0;
}


/*************************** llhttp callback functions ****************/

static int on_message_begin(llhttp_t* llhttp) {
	print_func;
	uv_httpd_client_t* client = llhttp->data; 
	mybuf_clear(&client->pkt); 
	reset_request(client);
	return 0;
}

static int on_url(llhttp_t* llhttp, const char* at, size_t length) {
	print_func;
	dnprintf(at, length, 1);
	uv_httpd_client_t* client = llhttp->data;
	client->req.url.offset = client->pkt.size;
	client->req.url.len = length;
	mybuf_append(&client->pkt, at, length);
	return 0;
}

static int on_status(llhttp_t* llhttp, const char* at, size_t length) {
	print_func;
	dnprintf(at, length, 1);
	return 0;
}

static int on_method(llhttp_t* llhttp, const char* at, size_t length) {
	print_func;
	dnprintf(at, length, 1);
	return 0;
}

static int on_version(llhttp_t* llhttp, const char* at, size_t length) {
	print_func;
	dnprintf(at, length, 1);
	uv_httpd_client_t* client = llhttp->data;
	client->req.version.offset = client->pkt.size;
	client->req.version.len = length;
	mybuf_append(&client->pkt, at, length);
	return 0;
}

static int on_header_field(llhttp_t* llhttp, const char* at, size_t length) {
	print_func;
	dnprintf(at, length, 1);
	uv_httpd_client_t* client = llhttp->data;
	headers_append_key(client, client->pkt.size, length);
	mybuf_append(&client->pkt, at, length);
	return 0;
}

static int on_header_value(llhttp_t* llhttp, const char* at, size_t length) {
	print_func;
	dnprintf(at, length, 1);
	uv_httpd_client_t* client = llhttp->data;
	headers_append_value(client, client->pkt.size, length);
	mybuf_append(&client->pkt, at, length);
	return 0;
}

static int on_chunk_extension_name(llhttp_t* llhttp, const char* at, size_t length) {
	print_func;
	dnprintf(at, length, 1);
	return 0;
}

static int on_chunk_extension_value(llhttp_t* llhttp, const char* at, size_t length) {
	print_func;
	dnprintf(at, length, 1);
	return 0;
}

static int on_headers_complete(llhttp_t* llhttp) {
	print_func;
	return 0;
}

static int on_body(llhttp_t* llhttp, const char* at, size_t length) {
	print_func;
	dnprintf(at, length, 1);
	uv_httpd_client_t* client = llhttp->data;
	client->req.body.offset = client->pkt.size;
	client->req.body.len = length;
	mybuf_append(&client->pkt, at, length);
	return 0;
}

static int on_message_complete(llhttp_t* llhttp) {
	print_func;
	uv_httpd_client_t* client = llhttp->data;
	client->req.base = client->pkt.buf;
	client->on_request(client->server, client, &client->req);
	if (!headers_contains(client, "Connection", "keep-alive")) {
		uv_close((uv_handle_t*)&client->tcp, on_close);
	}
	reset_request(client);
	return 0;
}

static int on_url_complete(llhttp_t* llhttp) {
	print_func;
	return 0;
}

static int on_status_complete(llhttp_t* llhttp) {
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
	print_func;
	return 0;
}

static int on_header_field_complete(llhttp_t* llhttp) {
	print_func;
	return 0;
}

static int on_header_value_complete(llhttp_t* llhttp) {
	print_func;
	return 0;
}

static int on_chunk_extension_name_complete(llhttp_t* llhttp) {
	print_func;
	return 0;
}

//static int on_chunk_extension_value_complete(llhttp_t* llhttp) {
//	print_func;
//	return 0;
//}

static int on_chunk_header(llhttp_t* llhttp) {
	print_func;
	return 0;
}

static int on_chunk_complete(llhttp_t* llhttp) {
	print_func;
	return 0;
}

static int on_reset(llhttp_t* llhttp) {
	print_func;
	return 0;
}

static void setup_default_llhttp_settings(llhttp_settings_t* settings) {
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
	assert(status == 0);
	struct write_req_t* wr = req->data;
	free(wr->buf.base);
	free(wr);
}

static void on_alloc(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
	print_func; dprintf("suggested_size=%zu\n", suggested_size);
	uv_httpd_client_t* client = handle->data;
	mybuf_reserve(&client->buf, DEFAULT_BUFF_SIZE);
	buf->base = client->buf.buf;
#ifdef _WIN32
	buf->len = (ULONG)mybuf_space(&client->buf);
#else
	buf->len = mybuf_space(&client->buf);
#endif
}

static void on_close(uv_handle_t* peer) {
	print_func;
	uv_httpd_client_t* client = peer->data;
	mybuf_clear(&client->buf);
	mybuf_clear(&client->pkt);
	reset_request(client);
	free(peer); // since our uv_tcpclient_t's first member is uv_tcp_t, so peer's addr IS our client's addr, just free it.
}

static void on_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
	print_func; dprintf("nread= %zd\n", nread);
	uv_httpd_client_t* client = stream->data;
	enum llhttp_errno parse_ret;

	if (nread < 0) {
		uv_close((uv_handle_t*)stream, on_close);
		return;
	} else if (nread == 0) {
		return;
	}

	dprintf("before llhttp_execute\n");
	dnprintf(buf->base, nread, 1);
	parse_ret = llhttp_execute(&client->parser, client->buf.buf, nread);
	if (parse_ret != HPE_OK) {
		fprintf(stderr, "Parse error: %s %s\n", llhttp_errno_name(parse_ret),
				client->parser.reason);
		uv_close((uv_handle_t*)stream, on_close);
	} else {
		// parse succeed, on_request_t should be called in on_message_complete		
	}
	dprintf("after llhttp_execute\n");
	mybuf_clear(&client->buf);
}

static int getpeeraddr(uv_tcp_t* tcp, char* ip, size_t len) {
	struct sockaddr_in addr;
	int addrlen = sizeof(addr);
	int r = uv_tcp_getpeername(tcp, (struct sockaddr*)&addr, &addrlen);
	if (r) {
		warn_on_uv_err(r, "uv_tcp_getpeername");
		return r;
	}
	r = uv_ip4_name(&addr, ip, len);
	if (r) {
		warn_on_uv_err(r, "uv_ip4_name");
	}
	if (len)
		ip[len - 1] = '\0';
	return r;
}

static void on_connected(uv_stream_t* stream, int status) {
	print_func;
	assert(status == 0);
	uv_httpd_server_t* server = stream->data;
	uv_httpd_client_t* client = malloc(sizeof * client);
	fatal_if_null(client);
	int r = uv_tcp_init(stream->loop, &client->tcp);
	fatal_on_uv_err(r, "uv_tcp_init failed");
	r = uv_accept(stream, (uv_stream_t*)&client->tcp);
	fatal_on_uv_err(r, "uv_accept error");

	client->server = server;
	client->on_request = server->on_request;
	client->tcp.data = client;
	llhttp_init(&client->parser, HTTP_REQUEST, &server->http_settings);
	client->parser.data = client;
	mybuf_init(&client->buf);
	mybuf_init(&client->pkt);
	client->req.headers.headers = client->headers;
	client->req.headers.n = 0;
	getpeeraddr(&client->tcp, client->req.ip, sizeof(client->req.ip));
	uv_read_start((uv_stream_t*)&client->tcp, on_alloc, on_read);
}


/*************************** public functions ****************/

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
}

int string_ncmp(const char* s1, size_t len1, const char* s2, size_t len2) {
	if (len1 == len2) {
		return memcmp(s1, s2, len1);
	}
	return 1;
}

int string0_ncmp(const char* s1, const char* s2, size_t len2) {
	size_t len1 = strlen(s1);
	if (len1 == len2) {
		return memcmp(s1, s2, len1);
	}
	return 1;
}

int string_nicmp(const char* s1, size_t len1, const char* s2, size_t len2) {
	if (len1 == len2) {
		int ca, cb;
		do {
			ca = *((unsigned char*)s1++);
			cb = *((unsigned char*)s2++);
			ca = tolower(toupper(ca));
			cb = tolower(toupper(cb));
		} while (ca == cb && ca != '\0' && len1--);
		return len1 != 0;
	}
	return 1;
}

int string0_nicmp(const char* s1, const char* s2, size_t len2) {
	size_t len1 = strlen(s1);
	if (len1 == len2) {
		return string_nicmp(s1, len1, s2, len2);
	}
	return 1;
}

void uv_httpd_enable_printf(int enable) {
	enable_print = enable;
}

int uv_httpd_create(uv_httpd_server_t** server, uv_loop_t* loop, on_request_t on_request) {
	int r = UV_ENOMEM;
	uv_httpd_server_t* s;

	s = malloc(sizeof(*s));
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
	uvlog_debug("uv_httpd.tcp closed");
}

void uv_httpd_stop(uv_httpd_server_t* server) {
	uv_close((uv_handle_t*)&server->tcp, on_server_closed);
}

void uv_httpd_free(uv_httpd_server_t* server) {
	free(server);
}

int uv_httpd_listen(uv_httpd_server_t* server, const char* ip, int port)
{
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

int uv_httpd_write_response(uv_httpd_client_t* client, char* response, size_t len)
{
	struct write_req_t* req = malloc(sizeof * req);
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
	return uv_write(&req->req, (uv_stream_t*)&client->tcp, &req->buf, 1, on_write);
}
