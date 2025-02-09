#include <gtest/gtest.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Operations.hpp"
#include "Common.hpp"

namespace Lust
{
	TEST(Transformation, Translation)
	{
		auto sample = glm::translate(glm::mat4(1.0f), glm::vec3(2.5f, 1.5f, 0.0f));
		auto sample2 = Translate<float>(Eigen::Matrix4f::Identity(), 2.5f, 1.5f, 0.0f);
		ValidateMatrices(sample, sample2);
	}
}