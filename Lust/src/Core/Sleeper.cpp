#include "Sleeper.hpp"

float Lust::Sleeper::s_FrameTime = 1.0f / 60.0f;
std::chrono::steady_clock::time_point Lust::Sleeper::s_StartTime;

void Lust::Sleeper::StartTracking()
{
	s_StartTime = std::chrono::steady_clock::now();
}

void Lust::Sleeper::Sleep()
{
	std::chrono::steady_clock::time_point now = std::chrono::high_resolution_clock::now();
	std::chrono::microseconds time = std::chrono::duration_cast<std::chrono::microseconds>(now - s_StartTime);
	float timeS = time.count() / 1e6f;
	while (timeS < s_FrameTime)
	{
		now = std::chrono::steady_clock::now();
		time = std::chrono::duration_cast<std::chrono::microseconds>(now - s_StartTime);
		timeS = time.count() / 1e6f;
	}
}

void Lust::Sleeper::SetFrameTime(float time)
{
	s_FrameTime = time;
}
