#pragma once

#include <LustSandboxDLLMacro.hpp>
#include <MathComponents.hpp>

namespace Lust
{
	namespace Sandbox
	{
		struct CompleteMVP
		{
			mat4 model;
			mat4 view;
			mat4 projection;
			mat4 mipLevel;
		};

		struct SmallMVP
		{
			mat4 model;
		};
	}
}
