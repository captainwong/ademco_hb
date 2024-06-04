#ifndef __HB_CONFIG_H__
#define __HB_CONFIG_H__

#ifdef HB_EXPORTS

#if defined __GNUC__
#define HB_EXPORT_SYMBOL __attribute__((visibility("default")))
#elif defined(_MSC_VER)
#define HB_EXPORT_SYMBOL __declspec(dllexport)
#else
#define HB_EXPORT_SYMBOL
#endif

#else  // HB_EXPORTS

#define HB_EXPORT_SYMBOL

#endif

#ifndef HB_ENABLE_TO_STRING
#ifdef ADEMCO_ENABLE_TO_STRING
#define HB_ENABLE_TO_STRING ADEMCO_ENABLE_TO_STRING
#else
#define HB_ENABLE_TO_STRING 1
#endif
#endif // HB_ENABLE_TO_STRING

#if HB_ENABLE_TO_STRING
#ifndef HB_ENABLE_CHINESE
#ifdef ADEMCO_ENABLE_CHINESE
#define HB_ENABLE_CHINESE ADEMCO_ENABLE_CHINESE
#else
#define HB_ENABLE_CHINESE 1
#endif
#endif
#else // HB_ENABLE_TO_STRING
#undef HB_ENABLE_CHINESE
#define HB_ENABLE_CHINESE 0
#endif // HB_ENABLE_TO_STRING

#endif
