#ifndef __ADEMCO_CONFIG_H__
#define __ADEMCO_CONFIG_H__

#include <libemb/libemb.h>

#ifdef ADEMCO_EXPORTS
#if defined __GNUC__
#define ADEMCO_EXPORT_SYMBOL __attribute__((visibility("default")))
#elif defined(_MSC_VER)
#define ADEMCO_EXPORT_SYMBOL __declspec(dllexport)
#else
#define ADEMCO_EXPORT_SYMBOL
#endif
#else  // ADEMCO_EXPORTS
#define ADEMCO_EXPORT_SYMBOL
#endif  // ADEMCO_EXPORTS

#if defined(SWIGJAVA) || defined(SWIGCSHARP)
typedef uint8_t ademco_char_t;
#else
typedef char ademco_char_t;
#endif

#ifndef ADEMCO_ENABLE_TO_STRING
#define ADEMCO_ENABLE_TO_STRING 1
#endif

#if ADEMCO_ENABLE_TO_STRING
#ifndef ADEMCO_ENABLE_CHINESE
#define ADEMCO_ENABLE_CHINESE 1
#endif
#else
#undef ADEMCO_ENABLE_CHINESE
#define ADEMCO_ENABLE_CHINESE 0
#endif

#ifndef ADEMCO_ENABLE_PARSE_ERROR
#define ADEMCO_ENABLE_PARSE_ERROR 1
#endif

#if ADEMCO_ENABLE_PARSE_ERROR
#define ADEMCO_FILL_PARSE_ERROR(err, offset_, msg_) \
    do {                                            \
        if (err) {                                  \
            err->file = __FILE__;                   \
            err->line = __LINE__;                   \
            err->offset = (offset_);                \
            err->msg = (msg_);                      \
        }                                           \
    } while (0);
#else
#define ADEMCO_FILL_PARSE_ERROR(err, offset_, msg_) \
    do {                                            \
        UNUSED(err);                                \
    } while (0)
#endif  // ADEMCO_ENABLE_PARSE_ERROR

// CRC16 table maybe too big for embedded system, user can use external CRC16 function,
// like read it from e2prom or flash to save code space.
// to enable this featuer, define `ADEMCO_USE_EXTERNAL_CRC16`, and implement `ademco_crc16` function.
#ifdef ADEMCO_USE_EXTERNAL_CRC16
#ifdef __cplusplus
extern "C" {
#endif
uint16_t ademco_crc16(const ademco_char_t* buf, size_t len);
#ifdef __cplusplus
}
#endif
#endif  // ADEMCO_USE_EXTERNAL_CRC16

#endif  // __ADEMCO_CONFIG_H__
