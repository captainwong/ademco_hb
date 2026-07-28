#ifndef __HB_CRYPTO_CONFIG_H__
#define __HB_CRYPTO_CONFIG_H__


#include <libademco/libademco.h>

#ifdef LIBHBCRYPTO_EXPORTS

#if defined __GNUC__
#define HBCRYPTO_EXPORT_SYMBOL __attribute__((visibility("default")))
#elif defined(_MSC_VER)
#define HBCRYPTO_EXPORT_SYMBOL __declspec(dllexport)
#else
#define HBCRYPTO_EXPORT_SYMBOL
#endif

#else  // LIBHBCRYPTO_EXPORTS

#define HBCRYPTO_EXPORT_SYMBOL

#endif

#endif /* __HB_CRYPTO_CONFIG_H__ */
