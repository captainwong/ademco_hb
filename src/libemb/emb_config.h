#ifndef __EMB_CONFIG_H__
#define __EMB_CONFIG_H__

#ifdef LIBEMB_EXPORTS
#if defined __GNUC__
#define EMB_EXPORT_SYMBOL __attribute__((visibility("default")))
#elif defined(_MSC_VER)
#define EMB_EXPORT_SYMBOL __declspec(dllexport)
#else
#define EMB_EXPORT_SYMBOL
#endif
#else  // LIBEMB_EXPORTS
#define EMB_EXPORT_SYMBOL
#endif  // LIBEMB_EXPORTS

//////////////////// bit ////////////////////

#define BIT7 0x80
#define BIT6 0x40
#define BIT5 0x20
#define BIT4 0x10
#define BIT3 0x08
#define BIT2 0x04
#define BIT1 0x02
#define BIT0 0x01

// bitn in BIT0, BIT1, ... , BIT7
#define set_bit(x, bitn) ((x) |= (bitn))
// bitn in BIT0, BIT1, ... , BIT7
#define clr_bit(x, bitn) ((x) &= ~(bitn))
// bitn in BIT0, BIT1, ... , BIT7
#define tst_bit(x, bitn) ((x) & (bitn))

//////////////////// helper macros ////////////////////

#ifndef count_of
#define count_of(x) (sizeof(x) / sizeof((x)[0]))
#endif  // count_of

#ifndef offset_of
#if defined(__GNUC__) || defined(__clang__)
#define offset_of(type, member) __builtin_offsetof(type, member)
#elif defined(_MSC_VER)
#define offset_of(type, member) ((size_t)&(((type*)0)->member))
#else
#define offset_of(type, member) ((size_t)((char*)&((type*)0)->member - (char*)0))
#endif
#endif  // offset_of

#ifndef container_of
#define container_of(ptr, type, member) \
    ((type*)((char*)(ptr) - offset_of(type, member)))
#endif  // container_of

/**
 * Flexible array helper macros, similar to kernel struct_size/flex_array_size usage.
 * Portable across GCC, Clang, and MSVC (no __typeof__ dependency).
 *
 * e.g. for a struct like:
 *   typedef struct {
 *     uint8_t count;
 *     item_t items[]; // flexible array
 *   } my_struct_t;
 *
 * you can use these macros to calculate sizes and get pointer to items:
 *    size_t size = struct_size(ptr, items, ptr->count); // total size
 *    const item_t* items_ptr = flex_array_data_cptr(ptr, my_struct_t, items, item_t); // pointer to items
 * these macros will do the right thing even if items is declared as item_t items[1] for older C standards.
 */

// calculate the size of the flexible array member
#define flex_array_size(ptr, member, count) ((size_t)(count) * sizeof((ptr)->member[0]))
// calculate the total size of the struct including the flexible array
#define struct_size(ptr, member, count) (sizeof(*(ptr)) + flex_array_size((ptr), member, (count)))
// get a pointer to the flexible array member
#define flex_array_data_ptr(ptr, type, member, elem_type) \
    ((elem_type*)((const uint8_t*)(ptr) + offset_of(type, member)))
// get a const pointer to the flexible array member
#define flex_array_data_cptr(ptr, type, member, elem_type) \
    ((const elem_type*)((const uint8_t*)(ptr) + offset_of(type, member)))

#ifndef UNUSED
#if defined(__C51__)
#define UNUSED(x) ((x) = (x))
#else
#define UNUSED(x) ((void)(x))
#endif  // __C51__
#endif  // UNUSED

#define _EMB_STRINGIFY_HELPER(x) #x
#define EMB_STRINGIFY(x) _EMB_STRINGIFY_HELPER(x)

#if defined(VSCODE)
// make vscode happy
// in the .vscode/c_cpp_properties.json, add "defines": ["VSCODE"]
// to suppress the errors/warnings
#ifndef __cplusplus
#define data
#endif
#define xdata
#define bdata
#define idata
#define pdata
#define code
#define small
#define sfr unsigned char
#define sbit bool
#define bit bool
#define reentrant
#define INTERRUPT(n)
#define USING(n)
#define declare_sfr(addr, name) sfr name = addr
#define declare_sbit(base, pin, name) sbit name
#define __xdata
#define __at(addr)
#elif defined(__C51__)
// now we are in C51 environment
#define INTERRUPT(n) interrupt n
#define USING(n) using n
#define declare_sfr(addr, name) sfr name = addr
#define declare_sbit(base, pin, name) sbit name = base ^ pin
#define __at(addr) _at_ addr
#elif defined(__SDCC)
// now we are in SDCC environment
#define data __data
#define xdata __xdata
#define bdata __bdata
#define idata __idata
#define pdata __pdata
#define code __code
#define bit __bit
#define reentrant __reentrant
#define small __small
#define INTERRUPT(n) __interrupt(n)
#define USING(n) __using(n)
#define declare_sfr(addr, name) __sfr __at(addr) name
#define declare_sbit(base, pin, name) __sbit __at(base + pin) name
#else
// now maybe using MSVC or GCC
#define sfr unsigned char
#define sbit bool
#define bit bool
#define reentrant
#define INTERRUPT(n)
#define USING(n)
#define declare_sfr(addr, name)
#define declare_sbit(base, pin, name)
#define __xdata
#define __at(addr)
#endif  // VSCODE

// armclang v6 `gmtime` always return NULL
#ifndef EMB_DISABLE_GMTIME
#ifdef __ARMCC_VERSION
#define EMB_DISABLE_GMTIME 1
#else  // __ARMCC_VERSION
#define EMB_DISABLE_GMTIME 0
#endif  // __ARMCC_VERSION
#endif  // EMB_DISABLE_GMTIME

// embedded system may not have enough stack space
// declare these macros to use static/unique buffer for big buf declared in functions
#ifndef EMB_USE_STATIC_BUF
#define EMB_USE_STATIC_BUF 0
#endif
#ifndef EMB_USE_UNIQUE_BUF
#define EMB_USE_UNIQUE_BUF 0
#endif

// under C51, we may need to use data for buffer
#if defined(__C51__) && !defined(EMB_DATA_MODIFIER)
#define EMB_DATA_MODIFIER data
#else
#define EMB_DATA_MODIFIER
#endif

// under C51, we may need to use bdata for buffer
#if defined(__C51__) && !defined(EMB_BDATA_MODIFIER)
#define EMB_BDATA_MODIFIER bdata
#else
#define EMB_BDATA_MODIFIER
#endif

// under C51, we need to use xdata for buffer
#if defined(__C51__) && !defined(EMB_XDATA_MODIFIER)
#define EMB_XDATA_MODIFIER xdata
#else
#define EMB_XDATA_MODIFIER
#endif

// under C51, we may need to use code for const data
#if defined(__C51__) && !defined(EMB_CODE_MODIFIER)
#define EMB_CODE_MODIFIER code
#else
#define EMB_CODE_MODIFIER
#endif

// Keil C51 and ICCAVR do not have stdint.h, time.h, stdbool.h, that's all we need
#if defined(__C51__) || defined(_AVR)

// stdint
#include "emb_stdint.h"

// bool
#ifndef __bool_true_false_are_defined

#if defined(__C51__) && defined(EMB_ENABLE_BIT_AS_BOOL)
// Even C51 can use bit as bool, but bool maybe used as function parameter or return value,
// that may cause bizarre problems, so be careful when you enable this option.
// To enable this option, define EMB_ENABLE_BIT_AS_BOOL in your project settings.
typedef bit bool;
#else  // use unsigned char as bool
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

#else  // suppose we are in a normal environment, and these headers are available
       // gcc-avr has these headers
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <time.h>
#endif  // __C51__ || _AVR

#if defined(__C51__) && defined(EMB_ENABLE_STDIO)
// C51 doesn't have `snprintf` in stdio.h
#define EMB_ENABLE_CUSTOM_STDIO
#include "emb_stdio.h"
#endif

#if defined(__C51__)
#if defined(EMB_ENABLE_INTTYPES)
// C51 doesn't have inttypes.h
#define EMB_ENABLE_CUSTOM_INTTYPES
#include "emb_inttypes.h"
#endif  // EMB_ENABLE_INTTYPES
#else
#include <inttypes.h>  // assume normal environment has inttypes.h
#endif

#ifdef EMB_ENABLE_CUSTOM_TIME
#include "emb_time.h"
#else
#include <time.h>
typedef time_t emb_time_t;
#define emb_tm tm
#define emb_localtime localtime
#define emb_localtime_r localtime_r
#define emb_time time
#define emb_mktime mktime
#define emb_gmtime gmtime
#define emb_gmtime_r gmtime_r
#endif

#endif /* __EMB_CONFIG_H__ */
