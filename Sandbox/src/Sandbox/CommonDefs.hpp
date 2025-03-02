#pragma once

#include <LustSandboxDLLMacro.hpp>

namespace Lust
{
	namespace Sandbox
	{
		struct CompleteMVP
		{
			Eigen::Matrix4f model;
			Eigen::Matrix4f view;
			Eigen::Matrix4f projection;
			Eigen::Matrix4f mipLevel;
		};

		struct SmallMVP
		{
			Eigen::Matrix4f model;
		};
	}
}
