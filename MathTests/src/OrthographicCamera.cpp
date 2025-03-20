#include <gtest/gtest.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Operations.hpp"
#include "Common.hpp"

namespace Lust
{
	TEST(Projection, Orthographic)
	{
		auto sample = glm::orthoLH_ZO(-640.0f, 640.0f, -360.0f, 360.0f, 0.0f, 1.0f);
		auto sample2 = Ortho(-640.0f, 640.0f, -360.0f, 360.0f, 0.0f, 1.0f);
		ValidateMatrices(sample, sample2);
	}

	TEST(Projection, ValidateRadiansToDegrees)
	{
		float radians = Lust::Radians(45.0f);
		float degrees = Lust::Degrees(radians);
		EXPECT_TRUE(degrees <= (45.0f + 0.000001f) && degrees >= (45.0f - 0.000001f));
	}
}