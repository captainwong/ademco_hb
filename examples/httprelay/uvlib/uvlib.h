#ifndef __UVLIB_H__
#define __UVLIB_H__

#pragma once

#include "list.h"
#include "mybuf.h"
#include "uv_httpc.h"
#include "uv_httpd.h"
#include "uv_log.h"
#include "uv_tcpclient.h"
#include "uv_tcpserver.h"
#include "uvlib_version.h"

#ifdef _WIN32

#define UVLIB_DIR "G:/dev_libs/ademco/"

#ifdef UVLIB_USE_MT
#define UVLIB_NAME "uvlib_mt.lib"
#else
#define UVLIB_NAME "uvlib.lib"
#endif

#ifdef _WIN64
#ifdef _DEBUG
#pragma comment(lib, UVLIB_DIR "x64/Debug/" UVLIB_NAME)
#else
#pragma comment(lib, UVLIB_DIR "x64/Release/" UVLIB_NAME)
#endif
#else  // WIN32
#ifdef _DEBUG
#pragma comment(lib, UVLIB_DIR "Debug/" UVLIB_NAME)
#else
#pragma comment(lib, UVLIB_DIR "Release/" UVLIB_NAME)
#endif
#endif

#endif  // _WIN32

#endif
