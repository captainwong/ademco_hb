#ifndef __HB_CHECK_CONFIG_H__
#define __HB_CHECK_CONFIG_H__

#include <libemb/libemb.h>

#ifdef LIBHBCHECK_EXPORTS

#if defined __GNUC__
#define HBCHECK_EXPORT_SYMBOL __attribute__((visibility("default")))
#elif defined(_MSC_VER)
#define HBCHECK_EXPORT_SYMBOL __declspec(dllexport)
#else
#define HBCHECK_EXPORT_SYMBOL
#endif

#else  // LIBHBCHECK_EXPORTS

#define HBCHECK_EXPORT_SYMBOL

#endif

#endif /* __HB_CHECK_CONFIG_H__ */
