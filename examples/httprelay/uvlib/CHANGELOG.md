libuvlib changelog
===============

## 1.2.2 2026-09-03

### uv_log: add `uv_log_get_file_size`

- add `uv_log_get_file_size()` API for application-side log rotation.
- returns `0` when no log file is configured (logging to stdout); returns `-1` on `uv_fs_stat` failure.

## 1.2.1 2026-08-23

### uv_httpd: graceful shutdown via `uv_shutdown`

- `on_message_complete`: close the connection using `uv_shutdown` (with `on_shutdown` callback) instead of calling `uv_close` directly. This drains pending writes before FIN, so large responses are no longer truncated mid-flight.
- new `on_shutdown`: logs shutdown errors and finally calls `uv_close`.
- remove `headers_contains`: keep-alive detection now uses `llhttp_should_keep_alive`, correctly handling HTTP/1.1 default keep-alive and HTTP/1.0 without keep-alive.

### uv_httpd: replace asserts with graceful error handling

- `on_write`: replace `assert(status == 0)` with a debug log. A write failure here is a normal event when the client disconnects mid-write (ECONNRESET/EPIPE/ECANCELED).
- `on_connected`: replace `assert(status == 0)` with a debug log + early `return`. An accept failure now logs and returns instead of aborting the process.

## 1.2.0 2026-07-14

### uv_httpd: client linked-list for graceful shutdown

- `uv_httpd_client_s` is now defined in `uv_httpd.h` (was in `.c`) with a `next` pointer for linked-list support.
- `uv_httpd_server_s` gains `clients` (linked list head) and `nclients` (count) fields.
- `on_connected`: new clients are added to the server's client linked list.
- `on_close`: clients are removed from the server's linked list before being freed.
- `uv_httpd_stop`: now iterates all active clients and calls `uv_close` on each, then closes the listen socket. This ensures `uv_run` can exit even when keep-alive clients are connected.
- `buf` and `pkt` are now heap-allocated `mybuf_t` (pointers via `void*` in the header) to support the public struct definition.

## 1.1.0 2026-07-14

### uv_tcpserver: pending-list architecture

This release introduces the **pending-list** mechanism for client lifecycle management.

#### New API

- `uv_tcpserver_take_cli(cli)` — Upper layer takes ownership of a client, removing it from the server's pending list. After this call, `uv_tcpserver_stop()` will NOT call `on_connect(cli, 0)` for this client.

#### Behavior Changes

- `on_connect` callback signature changed: return type from `void` to `int`.
  - Return `1` when `connected == 1`: upper layer takes ownership, client NOT added to pending list.
  - Return `0` when `connected == 1`: client stays in pending list (default).
  - Return value ignored when `connected == 0`.
- `on_read` error path (`nread < 0`): now calls `pending_remove()` + `on_connect(client, 0)` before `uv_close()`. Previously, `on_connect(client, 0)` was not called on read errors.
- `uv_tcpserver_stop()`: now iterates the pending list and calls `close_cli()` for each pending client, which triggers `on_connect(client, 0)` → `uv_close` → `on_close` → `free_cli_ctx()`.
- `uv_tcpserver_close_cli()`: now calls `pending_remove()` + `on_connect(client, 0)` before `uv_close()`.
- Internal `pending_remove()` uses pointer-to-pointer for safe linked-list deletion.

#### ⚠️ Breaking: DO NOT call `free_cli_ctx()` in `on_connect(client, 0)`

The library's `on_close` callback automatically calls `uv_tcpserver_free_cli_ctx()`. Calling it manually in your `on_connect(client, 0)` handler causes a double-free. Only clean up your own `client->data` in the disconnect handler.

#### Unit tests

Added 6 new tests covering:
- `take_cli` removes from pending list
- `on_connect` return 1 (taken) vs return 0 (pending)
- `uv_tcpserver_stop` closes pending clients
- Multiple pending clients stop
- `close_cli` removes from pending list

## 1.0.12 2026-07-08 19:02:00

### uv_httpc: defensive NULL guards

- fix crash when `httpc`/`loop`/`host`/`port`/`on_response` is NULL in `uv_httpc_create` — now rejects with `UV_EINVAL`
- fix crash when `httpc`/`url`/`data` is NULL in `uv_httpc_post` — now rejects with `UV_EINVAL`
- fix crash when `httpc` is NULL in `uv_httpc_stop` — returns 0 silently
- fix `strdup` OOM not checked in `uv_httpc_create` and `uv_httpc_post`

### uv_httpc: robustness

- add `stopped` flag to prevent double-stop in `uv_httpc_stop`
- fix `uv_write` failure in `send_request` causes `exit()` — now returns error via callback
- fix `uv_read_start` return value not checked in `on_connect` — propagate error on failure

### uv_httpd: defensive NULL guards & robustness

- fix crash when `server`/`loop`/`on_request` is NULL in `uv_httpd_create` — now rejects with `UV_EINVAL`
- fix crash when `client`/`response` is NULL in `uv_httpd_write_response` — now rejects with `UV_EINVAL`
- fix `on_connected` `uv_tcp_init`/`uv_accept` failure causes `exit()` (DoS) — now gracefully closes connection

### uv_log: fixes

- fix crash when NULL is passed to `uv_log_level_from_string` — returns `uv_log_level_invalid`
- fix `uv_log_raw` and `uv_log_hexdump` silently discarding logs with `uv_log_level_raw` flag (bitmask comparison)
- fix `uv_log_raw` now respects raw flag: raw mode skips timestamp/level prefix
- fix `localtime()` thread-safety: use `localtime_s` (Windows) / `localtime_r` (POSIX)

### uv_http: deprecation

- mark `nprintf` as deprecated (internal debug helper, to be removed)

## 1.0.11 2026-07-08 17:37:00

### uv_tcpclient: critical bug fixes

- fix crash when `settings` is NULL in `uv_tcpclient_new` — now rejects NULL with `UV_EINVAL`
- fix `strdup` return value not checked for OOM in `uv_tcpclient_start`
- fix `strcmp` instead of pointer comparison for `domain`/`port` dedup
- fix memory leak when `uv_write` fails in `uv_tcpclient_send`

### uv_tcpclient: robustness

- use `calloc` instead of `malloc` in `uv_tcpclient_new` for zero-initialized struct
- add `cb > 0` guard in `on_read` parse loop to prevent infinite loop
- use `suggested_size` in `on_alloc` instead of hardcoded 1024-byte buffer
- document correct `uv_tcpclient_stop` / `uv_tcpclient_free` lifecycle

### uv_tcpserver: critical bug fixes

- fix crash when `settings` is NULL in `uv_tcpserver_create` — now rejects NULL with `UV_EINVAL`
- fix `ptr->data` uninitialized in `uv_tcpserver_create` (malloc → calloc)
- fix uv handle leak in `on_connection` when `uv_accept` or `uv_read_start` fails (bare free → uv_close)
- fix `on_connection` field initialization order: `client->tcp.data` / `client->server` now set before `uv_read_start`

### uv_tcpserver: robustness

- add `cb > 0` guard in `on_read` parse loop to prevent infinite loop
- use `suggested_size` in `on_alloc` instead of hardcoded 1024-byte buffer
- document correct `uv_tcpserver_stop` / `uv_tcpserver_free` lifecycle

## 1.0.10 2026-07-08 17:08:00

### uv_httpd: critical bug fixes

- fix `on_read` passes inconsistent buffer pointer to `llhttp_execute` (was `client->buf.buf`, now correctly uses `buf->base`)
- fix `reset_request` uses fragile offset-based memset; replaced with explicit field-by-field reset
- fix memory leak when `uv_write` fails in `uv_httpd_write_response`

### uv_httpd: security & robustness

- add `llhttp_settings_init()` call before setting callbacks to avoid undefined behavior from uninitialized fields
- add `UV_HTTPD_MAX_BODY_SIZE` limit (default 10 MB, overridable via `#define`) to prevent memory exhaustion DoS
- check `uv_read_start()` return value in `on_connected`, close connection on failure
- use `suggested_size` in `on_alloc` instead of hardcoded 1024-byte buffer
- use `calloc` instead of `malloc` in `on_connected` and `uv_httpd_create` to ensure zero-initialized structs
- document correct `uv_httpd_stop` / `uv_httpd_free` lifecycle in header

### uv_http: bug fix

- fix `string_nicmp` out-of-bounds read when comparing two empty strings

## 1.0.9 2026-07-07 21:43:26

- add forward declare for `uv_httpd_request_t, uv_httpd_client_t`


## 1.0.8 2025-3-17 17:33:00

- move some functions to `uv_http.c`


## 1.0.7 2025-3-17 16:51:11

- add `uv_httpd_setup_on_client_closing` for user to handle the closing event of `uv_httpd_client_t`


## 1.0.6 2025-3-15 18:26:37

- `CFLAGS` add `-Wpedantic -Wextra -Werror -std=gnu99 -O2`


## 1.0.5 2024-2-1 14:31:53

- fix `on_parse` got error will skip the rest dat if exists


## 1.0.4 2024-1-13 23:22:26

- fix `uv_tcpclient_t`, `on_read`, `cb` not initialized


## 1.0.3 2024-1-12 17:21:01

- when `on_parse` got `uv_tcp_parse_error`, drop `*ate` bytes
- add `mybuf_to_string`, `mybuf_free`


## 1.0.2 2024-1-4 11:03:21

- `mybuf_append_hexdump` add option for new line


## 1.0.1 2023-12-29 20:27:38

- `mybuf_append_hexdump` add a seperator line below header


## 1.0.0 2023-12-27 17:40:31

- initial version
