#ifndef __HTTPRELAY_CONTROL_H__
#define __HTTPRELAY_CONTROL_H__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <uv.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int enabled;              /* 0: control interface disabled (legacy behavior) */
    int port;                 /* control http port */
    char listen_addr[64];     /* default "127.0.0.1" */
    char token[129];          /* empty = no auth (quick-verify mode) */
} control_settings_t;

/*
 * parse options from argv: --control_port=N | --control_port N
 *                           --listen_addr=A   | --listen_addr A
 *                           --token=T         | --token T
 * non-option args are collected positionally (0 = tcp_port, 1 = callback_uri).
 * return: number of positional args found; -1 on unknown option / bad value.
 */
int control_parse_args(int argc, char** argv, control_settings_t* settings, const char** positional);

/* start the control http server on uv_default_loop(). return 0 or uv_errno_t */
int control_start(const control_settings_t* settings);

/* upsert (acct -> client, ademco_id) in index, called by httprelay.c on every
 * machine packet with ademco_id, and on first packet to register acct. */
void control_index_update(const char* acct, void* cli, unsigned long ademco_id);

/* remove acct from index, called when machine tcp connection drops */
void control_index_remove(const char* acct);

#ifdef __cplusplus
}
#endif

#endif /* __HTTPRELAY_CONTROL_H__ */
