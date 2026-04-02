#ifndef __LIBHBCHECK_VERSION_H__
#define __LIBHBCHECK_VERSION_H__

/*
 * Versions with the same major number are ABI stable. API is allowed to
 * evolve between minor releases, but only in a backwards compatible way.
 */

#define LIBHBCHECK_VERSION_MAJOR 0
#define LIBHBCHECK_VERSION_MINOR 0
#define LIBHBCHECK_VERSION_PATCH 7
#define LIBHBCHECK_VERSION_IS_RELEASE 1
#define LIBHBCHECK_VERSION_SUFFIX ""

#define LIBHBCHECK_VERSION_HEX ((LIBHBCHECK_VERSION_MAJOR << 16) | \
                                (LIBHBCHECK_VERSION_MINOR << 8) |  \
                                (LIBHBCHECK_VERSION_PATCH))

#endif /* __LIBHBCHECK_VERSION_H__ */
