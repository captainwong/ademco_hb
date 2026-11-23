#ifndef __UVLIB_H__
#define __UVLIB_H__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "list.h"
#include "mybuf.h"
#include "uv_httpc.h"
#include "uv_httpd.h"
#include "uv_log.h"
#include "uv_tcpclient.h"
#include "uv_tcpserver.h"
#include "libuvlib_version.h"

#ifdef _WIN32

#ifdef UVLIB_LOAD_DLL

#define LIB_DIR "H:/dev_hb/libhb/vsprojects/"
#define LIB_NAME "libuvlib.lib"

#elif defined(UVLIB_LINK_STATIC)

#define LIB_DIR "H:/dev_hb/libhb/vsprojects/"
#define LIB_NAME "libuvlib_s.lib"

#elif defined(UVLIB_LINK_STATIC_MT)

#define LIB_DIR "H:/dev_hb/libhb/vsprojects/"
#define LIB_NAME "libuvlib_smt.lib"

#endif  // UVLIB_LOAD_DLL


#if defined(LIB_DIR) && defined(LIB_NAME)
#   ifdef _WIN64
#       ifdef _DEBUG
#           pragma comment(lib, LIB_DIR "x64/Debug/" LIB_NAME)
#       else
#           pragma comment(lib, LIB_DIR "x64/Release/" LIB_NAME)
#       endif
#   else  // WIN32
#       ifdef _DEBUG
#           pragma comment(lib, LIB_DIR "Debug/" LIB_NAME)
#       else
#           pragma comment(lib, LIB_DIR "Release/" LIB_NAME)
#       endif
#   endif /* _WIN64 */
#undef LIB_DIR
#undef LIB_NAME
#endif  // defined(LIB_DIR) && defined(LIB_NAME)

#endif  // _WIN32

#endif
