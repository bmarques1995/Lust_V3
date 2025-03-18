#include "Timestep.hpp"

Lust::Timestep::Timestep(float time)
	: m_Time(time)
{
}

Lust::Timestep::operator float() const
{ 
	return m_Time; 
}

float Lust::Timestep::GetSeconds() const
{ 
	return m_Time;
}

float Lust::Timestep::GetMilliseconds() const
{ 
	return m_Time * 1000.0f; 
}