#pragma once

/**
* @file MathBuffer.hpp
* @brief Contains functions to copy matrices to a buffer
*/

#include <Eigen/Eigen>
#include <cstring>

namespace Lust
{
	/**
	* Copies a matrix to a buffer
	* @param matrix Matrix source to copy
	* @param buffer Buffer to copy to
	* @param start Start offset
	*/
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