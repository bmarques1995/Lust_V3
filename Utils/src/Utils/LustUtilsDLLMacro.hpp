#pragma once

#ifdef LUST_UTILS_BUILD_DLL
#define LUST_UTILS_API __declspec(dllexport)
#elif defined(LUST_UTILS_CONSUME_DLL)
#define LUST_UTILS_API __declspec(dllimport)
#else
#define LUST_UTILS_API
#endif
