#pragma once

#include <Eigen/Eigen>
#include <cstring>

namespace Lust
{
	template<typename T>
	void CopyMatrix4ToBuffer(Eigen::Matrix4<T> matrix, uint8_t** buffer, size_t start)
	{
		for (size_t i = 0; i < 4; i++)
		{
			for (size_t j = 0; j < 4; j++)
			{

				memcpy(*buffer + (start + sizeof(T) * (i*4+j)), &matrix(i, j), sizeof(T));
			}
		}
	}
}