#pragma once

#ifdef LUST_SHADER_MNG_BUILD_DLL
#define LUST_SHADER_MNG_API __declspec(dllexport)
#elif defined(LUST_SHADER_MNG_CONSUME_DLL)
#define LUST_SHADER_MNG_API __declspec(dllimport)
#else
#define LUST_SHADER_MNG_API
#endif
