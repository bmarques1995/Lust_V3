#pragma once

#ifdef LUST_UI_BUILD_DLL
#define LUST_UI_API __declspec(dllexport)
#elif defined(LUST_UI_CONSUME_DLL)
#define LUST_UI_API __declspec(dllimport)
#else
#define LUST_UI_API
#endif
