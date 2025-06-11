#pragma once

#include <MathComponents.hpp>

namespace Lust
{
	scaling3 scaling(float x, float y, float z)
	{
		return Eigen::Scaling(x, y, z);
	}
}