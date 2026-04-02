#ifndef __LIBEMB_H__
#define __LIBEMB_H__

#include <libemb/emb_config.h>
#include <libemb/emb_bitrev.h>
#include <libemb/emb_version.h>

#ifdef _WIN32

#ifdef EMB_LOAD_DLL
#   define LIB_DIR "H:/dev_hb/libhb/vsprojects/"
#   define LIB_NAME "libemb.lib"
#elif defined(EMB_LINK_STATIC)
#   define LIB_DIR "H:/dev_hb/libhb/vsprojects/"
#   define LIB_NAME "libemb_s.lib"
#elif defined(EMB_LINK_STATIC_MT)
#   define LIB_DIR "H:/dev_hb/libhb/vsprojects/"
#   define LIB_NAME "libemb_smt.lib"
#endif  // EMB_LOAD_DLL

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

#endif /* __LIBEMB_H__ */
