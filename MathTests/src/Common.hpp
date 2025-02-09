#pragma once

#include <gtest/gtest.h>
#include <glm/glm.hpp>
#include <Eigen/Eigen>

namespace Lust
{
	template<typename T>
	void ValidateMatrices(const glm::mat<4, 4, T>& matrix, const Eigen::Matrix<T, 4, 4> eigenMatrix)
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				EXPECT_EQ(matrix[i][j], eigenMatrix(i, j));
			}
		}
	}
}