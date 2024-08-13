#ifndef __ADEMCO_CONFIG_H__
#define __ADEMCO_CONFIG_H__

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

#ifndef UNUSED
#ifdef __C51__
#define UNUSED(x) ((x) = (x))
#else
#define UNUSED(x) ((void)(x))
#endif  // __C51__
#endif  // UNUSED

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

// armclang v6 `gmtime` always return NULL
#ifndef ADEMCO_DISABLE_GMTIME
#ifdef __ARMCC_VERSION
#define ADEMCO_DISABLE_GMTIME 1
#else  // __ARMCC_VERSION
#define ADEMCO_DISABLE_GMTIME 0
#endif  // __ARMCC_VERSION
#endif  // ADEMCO_DISABLE_GMTIME

// embedded system may not have enough stack space
// declare these macros to use static/unique buffer for parse/make functions
#ifndef ADEMCO_USE_STATIC_BUF
#define ADEMCO_USE_STATIC_BUF 0
#endif
#ifndef ADEMCO_USE_UNIQUE_BUF
#define ADEMCO_USE_UNIQUE_BUF 0
#endif

// under C51, we need to use xdata for buffer
#if defined(__C51__) && !defined(ADEMCO_BUF_MEMORY_MODIFIER)
#define ADEMCO_BUF_MEMORY_MODIFIER xdata
#else
#define ADEMCO_BUF_MEMORY_MODIFIER
#endif

// under C51, we may need to use code for const data
#if defined(__C51__) && !defined(ADEMCO_CONST_MODIFIER)
#define ADEMCO_CONST_MODIFIER code
#else
#define ADEMCO_CONST_MODIFIER
#endif

// make this lib can be used in C51
#ifdef __C51__
#define INTERRUPT(n) interrupt n
#define USING(n) using n
#define declare_sbit(pin, name) sbit name = pin
#ifdef VSCODE
// make vscode happy
// in the .vscode/c_cpp_properties.json, add "defines": ["VSCODE"]
// to suppress the warning
#define data
#define xdata
#define bdata
#define idata
#define pdata
#define code
#define small
#define sfr unsigned char
#define sbit unsigned char
#define bit unsigned char
#define reentrant
#define INTERRUPT(n)
#define USING(n)
#define declare_sbit(pin, name) sbit name = 0
#endif  // VSCODE
#endif

// Keil C51 and ICCAVR do not have stdint.h, time.h, stdbool.h, that's all we need
#if defined(__C51__) || defined(_AVR)

// stdint
#include "ademco_stdint.h"

// bool
#ifndef __bool_true_false_are_defined

#ifdef __C51__  // C51 can use bit as bool
typedef bit bool;
#else  // AVR can use unsigned char as bool
typedef unsigned char bool;
#endif

#define true 1
#define false 0

#define __bool_true_false_are_defined
#endif  // __bool_true_false_are_defined

// NULL
#ifndef NULL
#define NULL ((void*)0)
#endif

// we need our own implementation of time.h
#define ADMECO_ENABLE_CUSTOM_TIME
#include "ademco_time.h"

#else  // suppose we are in a normal environment, and these headers are available
       // gcc-avr has these headers
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <time.h>
#endif  // __C51__ || _AVR

// C51 doesn't have the with `n` version functions in stdio.h
#if defined(__C51__) && defined(ADEMCO_ENABLE_STDIO)
#define ADEMCO_ENABLE_CUSTOM_STDIO
#include "ademco_stdio.h"
#endif  // defined(__C51__) && defined(ADEMCO_ENABLE_STDIO)

// CRC16 table maybe too big for embedded system, user can use external CRC16 function,
// like read it from e2prom or flash to save code space.
// to enable this featuer, define `ADEMCO_USE_EXTERNAL_CRC16`, and implement `ademco_crc16` function.
#ifdef ADEMCO_USE_EXTERNAL_CRC16
#ifdef __cplusplus
extern "C" {
#endif
uint16_t ademco_crc16(const ademco_char_t ADEMCO_BUF_MEMORY_MODIFIER* buf, size_t len);
#ifdef __cplusplus
}
#endif
#endif // ADEMCO_USE_EXTERNAL_CRC16

#endif  // __ADEMCO_CONFIG_H__
