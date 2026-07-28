#ifndef __EMB_VERSION_H__
#define __EMB_VERSION_H__

#include <libemb/emb_config.h>

#ifdef __cplusplus
extern "C" {
#endif

#if !(defined(__C51__) || defined(__SDCC)) || defined(VSCODE)
#pragma pack(push, 1)
#endif

/*
 * Versions with the same major number are ABI stable. API is allowed to
 * evolve between minor releases, but only in a backwards compatible way.
 */

#define EMB_VERSION_MAJOR 0
#define EMB_VERSION_MINOR 0
#define EMB_VERSION_PATCH 7
#define EMB_VERSION_IS_RELEASE 1
#define EMB_VERSION_SUFFIX ""

#define EMB_VERSION_HEX (((uint32_t)EMB_VERSION_MAJOR << 16) | \
                         ((uint32_t)EMB_VERSION_MINOR << 8) |  \
                         ((uint32_t)EMB_VERSION_PATCH))

EMB_EXPORT_SYMBOL uint32_t emb_version(void);
EMB_EXPORT_SYMBOL const char* emb_version_string(void);

#if !(defined(__C51__) || defined(__SDCC)) || defined(VSCODE)
#pragma pack(pop)
#endif

#ifdef __cplusplus
}
#endif

#endif /* __EMB_VERSION_H__ */
