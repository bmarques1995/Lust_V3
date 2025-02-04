#pragma once

#ifdef LUST_SANDBOX_BUILD_DLL
#define LUST_SANDBOX_API __declspec(dllexport)
#elif defined(LUST_SANDBOX_CONSUME_DLL)
#define LUST_SANDBOX_API __declspec(dllimport)
#else
#define LUST_SANDBOX_API
#endif
