#ifndef __ADEMCO_VERSION_H__
#define __ADEMCO_VERSION_H__

/*
 * Versions with the same major number are ABI stable. API is allowed to
 * evolve between minor releases, but only in a backwards compatible way.
 */

#define ADEMCO_VERSION_MAJOR 2
#define ADEMCO_VERSION_MINOR 1
#define ADEMCO_VERSION_PATCH 6
#define ADEMCO_VERSION_IS_RELEASE 1
#define ADEMCO_VERSION_SUFFIX ""

#define ADEMCO_VERSION_HEX ((ADEMCO_VERSION_MAJOR << 16) | \
                            (ADEMCO_VERSION_MINOR << 8) |  \
                            (ADEMCO_VERSION_PATCH))

#endif /* __ADEMCO_VERSION_H__ */
