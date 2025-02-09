#pragma once

#include <Eigen/Eigen>

namespace Lust
{
	template<typename T>
	Eigen::Matrix<T, 4, 4> Translate(const Eigen::Matrix<T, 4, 4>& inputMatrix, const T x, const T y, const T z)
	{
		Eigen::Matrix<T, 4, 4> matrix = inputMatrix;
		matrix(3, 0) = x;
		matrix(3, 1) = y;
		matrix(3, 2) = z;
		return matrix;
	}

	template<typename T>
	Eigen::Matrix<T, 4, 4> Translate(const Eigen::Matrix<T, 4, 4>& inputMatrix, const Eigen::Vector3<T>& pos)
	{
		Eigen::Matrix<T, 4, 4> matrix = inputMatrix;
		matrix(3, 0) = pos(0);
		matrix(3, 1) = pos(1);
		matrix(3, 2) = pos(2);
		return matrix;
	}

	template<typename T>
	Eigen::Matrix<T, 4, 4> Rotate(const Eigen::Matrix<T, 4, 4>& inputMatrix, const Eigen::Quaternion<T>& rotation)
	{
		Eigen::Matrix3f rotationMatrix = rotation.toRotationMatrix();
		Eigen::Matrix4f transformationMatrix = Eigen::Matrix4f::Identity();
		transformationMatrix.block<3, 3>(0, 0) = rotationMatrix;

		Eigen::Matrix<T, 4, 4> matrix = transformationMatrix * inputMatrix;
		return matrix;
	}

	template<typename T>
	Eigen::Matrix<T, 4, 4> Scale(const Eigen::Matrix<T, 4, 4>& inputMatrix, const T x, const T y, const T z)
	{
		Eigen::Matrix<T, 4, 4> matrix = inputMatrix;
		matrix(0, 0) = x;
		matrix(1, 1) = y;
		matrix(2, 2) = z;
		return matrix;
	}

	template<typename T>
	Eigen::Matrix<T, 4, 4> Scale(const Eigen::Matrix<T, 4, 4>& inputMatrix, const Eigen::Vector3<T>& scale)
	{
		Eigen::Matrix<T, 4, 4> matrix = inputMatrix;
		matrix(0, 0) = scale(0);
		matrix(1, 1) = scale(1);
		matrix(2, 2) = scale(2);
		return matrix;
	}
}
