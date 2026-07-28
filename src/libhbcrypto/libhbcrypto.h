#ifndef __LIBHBCRYPTO_H__
#define __LIBHBCRYPTO_H__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <libhbcrypto/hbcrypto.h>

#ifdef _WIN32

#if defined(HBCRYPTO_LOAD_DLL)

#define LIB_DIR "H:/dev_hb/libhb/vsprojects/"
#define LIB_NAME "libhbcrypto.lib"

#elif defined(HBCRYPTO_LINK_STATIC)

#define LIB_DIR "H:/dev_hb/libhb/vsprojects/"
#define LIB_NAME "libhbcrypto_s.lib"

#elif defined(HBCRYPTO_LINK_STATIC_MT)

#define LIB_DIR "H:/dev_hb/libhb/vsprojects/"
#define LIB_NAME "libhbcrypto_smt.lib"

#endif  // ADEMCO_LOAD_DLL

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

#endif  // __LIBHBCRYPTO_H__
