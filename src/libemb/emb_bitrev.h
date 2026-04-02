#ifndef __EMB_BITREV_H__
#define __EMB_BITREV_H__

#include <libemb/emb_config.h>

#ifdef __cplusplus
extern "C" {
#endif

#if !(defined(__C51__) || defined(__SDCC)) || defined(VSCODE)
#pragma pack(push, 1)
#endif

//////////////////// bcd ////////////////////

#define bcd_encode(x) (((x) / 10) << 4 | ((x) % 10))
#define bcd_decode(x) (((x) >> 4) * 10 + ((x) & 0x0F))

/////////////////// byte order reverse //////////////////

#define rev16(u) (uint16_t)((((uint16_t)(u) & 0xFF) << 8) | (((uint16_t)(u) & 0xFF00) >> 8))
#define rev32(u) (uint32_t)((((uint32_t)(u) & 0xFF) << 24) |    \
                            (((uint32_t)(u) & 0xFF00) << 8) |   \
                            (((uint32_t)(u) & 0xFF0000) >> 8) | \
                            (((uint32_t)(u) & 0xFF000000) >> 24))

/////////////////// bit reverse //////////////////

EMB_EXPORT_SYMBOL uint8_t bitrev8(uint8_t b);
EMB_EXPORT_SYMBOL uint16_t bitrev16(uint16_t b);
EMB_EXPORT_SYMBOL uint32_t bitrev32(uint32_t b);

#if !(defined(__C51__) || defined(__SDCC)) || defined(VSCODE)
#pragma pack(pop)
#endif

#ifdef __cplusplus
}
#endif

#endif /* __EMB_BITREV_H__ */
