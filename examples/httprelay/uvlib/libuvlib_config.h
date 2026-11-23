#ifndef __UVLIB_CONFIG_H__
#define __UVLIB_CONFIG_H__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifdef UVLIB_EXPORTS

#if defined __GNUC__
#define UVLIB_EXPORT_SYMBOL __attribute__((visibility("default")))
#elif defined(_MSC_VER)
#define UVLIB_EXPORT_SYMBOL __declspec(dllexport)
#else
#define UVLIB_EXPORT_SYMBOL
#endif

#else

#define UVLIB_EXPORT_SYMBOL

#endif

#endif
