#include "uvlib_version.h"

#define UVLIB_STRINGIFY(v) UVLIB_STRINGIFY_HELPER(v)
#define UVLIB_STRINGIFY_HELPER(v) #v

#define UVLIB_VERSION_STRING_BASE  UVLIB_STRINGIFY(UVLIB_VERSION_MAJOR) "." \
                                    UVLIB_STRINGIFY(UVLIB_VERSION_MINOR) "." \
                                    UVLIB_STRINGIFY(UVLIB_VERSION_PATCH)

#if UVLIB_VERSION_IS_RELEASE
# define UVLIB_VERSION_STRING  UVLIB_VERSION_STRING_BASE
#else
# define UVLIB_VERSION_STRING  UVLIB_VERSION_STRING_BASE "-" UVLIB_VERSION_SUFFIX
#endif

unsigned int uvlib_version(void) {
    return UVLIB_VERSION_HEX;
}

const char* uvlib_version_string(void) {
    return UVLIB_VERSION_STRING;
}
