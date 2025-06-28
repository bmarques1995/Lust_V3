#pragma once
#include "LustSandboxDLLMacro.hpp"
#include <Application.hpp>

namespace Lust
{
	class LUST_SANDBOX_API SandCoffin : public Application
	{
	public:
		SandCoffin(int argc, char** argv);
		~SandCoffin();
	};
}
