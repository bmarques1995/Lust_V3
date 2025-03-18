#pragma once

#include <chrono>
#include "LustDLLMacro.hpp"

namespace Lust
{
	/**
	* @brief %Sleeper class, handles sleeping for a set amount of time
	* @details This is necessary specially for Vulkan, that doesn't control the framerate with the VSync activated
	*/
	class LUST_API Sleeper
	{
	public:
		/**
		* @brief StartTracking starts the tracking of the time
		* @details Tracks the start runtime, that will be used to sleep
		*/
		static void StartTracking();
		/**
		* @brief Sleep sleeps for a set amount of time
		* @details until the frame time is reached
		*/
		static void Sleep();

		/**
		* @brief SetFrameTime sets the frame time, e.g. 1.0f / 60.0f for 60FPS.
		* @param time frame time
		*/
		static void SetFrameTime(float time);
	private:
		static float s_FrameTime;
		static std::chrono::steady_clock::time_point s_StartTime;
	};
}