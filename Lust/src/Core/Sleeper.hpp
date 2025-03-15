#pragma once

#include <chrono>
#include "LustDLLMacro.hpp"

namespace Lust
{
	class LUST_API Sleeper
	{
	public:
		static void StartTracking();
		static void Sleep();

		static void SetFrameTime(float time);
	private:
		static float s_FrameTime;
		static std::chrono::steady_clock::time_point s_StartTime;
	};
}