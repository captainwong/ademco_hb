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

#endif // ADEMCO_EXPORTS

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
#define ADEMCO_FILL_PARSE_ERROR(err, offset_, msg_)
#endif // ADEMCO_ENABLE_PARSE_ERROR

#ifdef ADEMCO_USE_CUSTOM_TIME_T
extern time_t ademco_get_time_t(time_t* t);
#endif // ADEMCO_USE_CUSTOM_TIME_T

// armclang v6 `gmtime` always return NULL
#ifndef ADEMCO_DISABLE_GMTIME
#define ADEMCO_DISABLE_GMTIME 0
#endif

// embedded system may not have enough stack space
#ifndef ADEMCO_USE_STATIC_BUF
#define ADEMCO_USE_STATIC_BUF 0
#endif

#ifndef ADEMCO_USE_UNIQUE_BUF
#define ADEMCO_USE_UNIQUE_BUF 0
#endif

#endif
