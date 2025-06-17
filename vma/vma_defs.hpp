#pragma once

#ifdef LUST_VMA_BUILD_DLL
#define VMA_CALL_PRE __declspec(dllexport)
#elif defined(LUST_VMA_CONSUME_DLL)
#define VMA_CALL_PRE __declspec(dllimport)
#else
#define VMA_CALL_PRE
#endif