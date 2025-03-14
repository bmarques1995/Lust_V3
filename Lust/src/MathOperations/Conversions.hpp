#pragma once

#include <limits>

namespace Lust
{
	template<typename genType>
	inline constexpr genType Radians(genType degrees)
	{
		static_assert(std::numeric_limits<genType>::is_iec559 || 0, "'radians' only accept floating-point input");

		return degrees * static_cast<genType>(0.01745329251994329576923690768489);
	}

	template<typename genType>
	inline constexpr genType Degrees(genType radians)
	{
		static_assert(std::numeric_limits<genType>::is_iec559 || 0, "'degrees' only accept floating-point input");

		return radians * static_cast<genType>(57.295779513082320876798154814105);
	}
}