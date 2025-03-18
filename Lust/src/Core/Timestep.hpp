#pragma once

#include "LustDLLMacro.hpp"

namespace Lust {

	class LUST_API Timestep
	{
	public:
		Timestep(float time = 0.0f);

		operator float() const;

		float GetSeconds() const;
		float GetMilliseconds() const;
	private:
		float m_Time;
	};

}