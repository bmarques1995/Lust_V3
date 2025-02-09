#pragma once

#include <Eigen/Eigen>

namespace Lust
{
	template<typename T>
	Eigen::Matrix<T, 4, 4> Ortho(const T left, const T right, const T bottom, const T top, const T zNear, const T zFar)
	{
		Eigen::Matrix<T, 4, 4> matrix = Eigen::Matrix<T, 4, 4>::Identity();
		matrix(0,0) = static_cast<T>(2) / (right - left);
		matrix(1,1) = static_cast<T>(2) / (top - bottom);
		matrix(2,2) = -static_cast<T>(1) / (zNear - zFar);
		matrix(3,0) = -(right + left) / (right - left);
		matrix(3,1) = -(top + bottom) / (top - bottom);
		matrix(3,2) = -zNear / (zNear - zFar);
		return matrix;
	}
}
