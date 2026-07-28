#ifndef __LIBHBCRYPTO_VERSION_H__
#define __LIBHBCRYPTO_VERSION_H__

/*
 * Versions with the same major number are ABI stable. API is allowed to
 * evolve between minor releases, but only in a backwards compatible way.
 */

#define LIBHBCRYPTO_VERSION_MAJOR 0
#define LIBHBCRYPTO_VERSION_MINOR 0
#define LIBHBCRYPTO_VERSION_PATCH 2
#define LIBHBCRYPTO_VERSION_IS_RELEASE 1
#define LIBHBCRYPTO_VERSION_SUFFIX ""

#define LIBHBCRYPTO_VERSION_HEX ((LIBHBCRYPTO_VERSION_MAJOR << 16) | \
                                 (LIBHBCRYPTO_VERSION_MINOR << 8) |  \
                                 (LIBHBCRYPTO_VERSION_PATCH))

#endif /* __LIBHBCRYPTO_VERSION_H__ */
