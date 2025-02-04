#pragma once

#ifdef LUST_BUILD_DLL
#define LUST_API __declspec(dllexport)
#define IMGUI_API __declspec(dllexport)
#elif defined(LUST_CONSUME_DLL)
#define LUST_API __declspec(dllimport)
#define IMGUI_API __declspec(dllimport)
#else
#define LUST_API
#define IMGUI_API
#endif

#define IMGUI_IMPL_API
