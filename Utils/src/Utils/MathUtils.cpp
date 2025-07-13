#include "MathUtils.hpp"

Lust::scaling3 Lust::scaling(float x, float y, float z)
{
	return Eigen::Scaling(x, y, z);
}
