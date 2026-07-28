#ifndef __LIBHB_VERSION_H__
#define __LIBHB_VERSION_H__

/*
 * Versions with the same major number are ABI stable. API is allowed to
 * evolve between minor releases, but only in a backwards compatible way.
 */

#define LIBHB_VERSION_MAJOR 3
#define LIBHB_VERSION_MINOR 7
#define LIBHB_VERSION_PATCH 3
#define LIBHB_VERSION_IS_RELEASE 1
#define LIBHB_VERSION_SUFFIX ""

#define LIBHB_VERSION_HEX (((uint32_t)LIBHB_VERSION_MAJOR << 16) | \
                            ((uint32_t)LIBHB_VERSION_MINOR << 8) |  \
                            ((uint32_t)LIBHB_VERSION_PATCH))

#endif /* __LIBHB_VERSION_H__ */
