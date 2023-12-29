#ifndef __UVLIB_VERSION_H__
#define __UVLIB_VERSION_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Versions with the same major number are ABI stable. API is allowed to
 * evolve between minor releases, but only in a backwards compatible way.
 */

#define UVLIB_VERSION_MAJOR 1
#define UVLIB_VERSION_MINOR 0
#define UVLIB_VERSION_PATCH 0
#define UVLIB_VERSION_IS_RELEASE 1
#define UVLIB_VERSION_SUFFIX ""

#define UVLIB_VERSION_HEX ((UVLIB_VERSION_MAJOR << 16) | \
                           (UVLIB_VERSION_MINOR << 8) |  \
                           (UVLIB_VERSION_PATCH))

unsigned int uvlib_version(void);
const char* uvlib_version_string(void);

#ifdef __cplusplus
}
#endif

#endif /* __UVLIB_VERSION_H__ */
