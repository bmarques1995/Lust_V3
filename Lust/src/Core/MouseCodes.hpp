#pragma once
#include <cstdint>
#include "LustDLLMacro.hpp"

namespace Lust
{
	using MouseCode = uint16_t;

	namespace Mouse
	{
		enum : MouseCode
		{
			LUST_BUTTON_LEFT = 0,
			LUST_BUTTON_MIDDLE = 1,
			LUST_BUTTON_RIGHT = 2,
			LUST_BUTTON_X1 = 3,
			LUST_BUTTON_X2 = 4
		};
	}
}