#ifndef __UV_TCP_H__
#define __UV_TCP_H__

#pragma once

#include <uv.h>


#ifdef __cplusplus
extern "C" {
#endif


typedef enum {
	uv_tcp_parse_ok = 0,
	uv_tcp_parse_not_enough,
	uv_tcp_parse_error,
}uv_tcp_parse_result_t;



#ifdef __cplusplus
}
#endif


#endif
