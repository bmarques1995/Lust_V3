#pragma once

#include "LustDLLMacro.hpp"

namespace Lust
{
	/**
	* @brief %Timestep class, handles time ellapsed in seconds
	*/
	class LUST_API Timestep
	{
	public:
		/**
		* @brief Timestep constructor
		*/
		Timestep(float time = 0.0f);

		/**
		* @brief Operator to get the time as a float, without functions
		*/
		operator float() const;

		/**
		* @brief GetSeconds retrieves the time ellapsed in seconds
		*/
		float GetSeconds() const;
		/**
		* @brief GetMilliseconds retrieves the time ellapsed in milliseconds
		*/
		float GetMilliseconds() const;
	private:
		float m_Time;
	};

}