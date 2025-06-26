#pragma once

#ifdef LUST_USES_WINDOWS

#include "LustDLLMacro.hpp"

namespace Lust
{
	LUST_API void StartCmdLine(int* out_argc, std::vector<char*>* out_argv);
}

#endif