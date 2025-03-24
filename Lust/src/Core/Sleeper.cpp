#include "Sleeper.hpp"
#include "Console.hpp"

float Lust::Sleeper::s_FrameTimeF = 1.0f / 60.0f;
double Lust::Sleeper::s_FrameTime = 1.0 / 60.0;
std::chrono::steady_clock::time_point Lust::Sleeper::s_StartTime = std::chrono::steady_clock::now();
std::chrono::steady_clock::time_point Lust::Sleeper::s_LastStartTime = std::chrono::steady_clock::now();
std::chrono::steady_clock::time_point Lust::Sleeper::s_EndTime = std::chrono::steady_clock::now();

void Lust::Sleeper::StartTracking()
{
	s_StartTime = std::chrono::steady_clock::now();
}

void Lust::Sleeper::Sleep()
{
	s_LastStartTime = s_StartTime;
	s_EndTime = std::chrono::steady_clock::now();
	std::chrono::microseconds time = std::chrono::duration_cast<std::chrono::microseconds>(s_EndTime - s_StartTime);
	double timeS = time.count() / 1e6;
	while (timeS < s_FrameTime)
	{
		s_EndTime = std::chrono::steady_clock::now();
		time = std::chrono::duration_cast<std::chrono::microseconds>(s_EndTime - s_StartTime);
		timeS = time.count() / 1e6f;
	}
}

void Lust::Sleeper::SetFrameTime(double time)
{
	s_FrameTime = time;
}

void Lust::Sleeper::SetFrameTimeF(float time)
{
	s_FrameTimeF = time;
}

double Lust::Sleeper::GetFrameTime()
{
	return s_FrameTime;
}

float Lust::Sleeper::GetFrameTimeF()
{
	return s_FrameTime;
}

double Lust::Sleeper::GetDeltaTime()
{
	std::chrono::microseconds time = std::chrono::duration_cast<std::chrono::microseconds>(s_EndTime - s_LastStartTime);
	double timeS = time.count() / 1e6;
	Console::CoreDebug("Delta time: {}", timeS);
	return timeS;
}

float Lust::Sleeper::GetDeltaTimeF()
{
	std::chrono::microseconds time = std::chrono::duration_cast<std::chrono::microseconds>(s_EndTime - s_LastStartTime);
	float timeS = time.count() / 1e6;
	Console::CoreDebug("Delta time: {}", timeS);
	return timeS;
}

std::chrono::steady_clock::time_point Lust::Sleeper::GetCurrentFrameTime()
{
	return s_StartTime;
}
