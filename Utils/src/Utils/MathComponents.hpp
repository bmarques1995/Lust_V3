#pragma once

#include <cstdint>
#include <Eigen/Eigen>

namespace Lust
{
	template<typename T, int Rows, int Cols>
	using tmat = Eigen::Matrix<T, Rows, Cols>;
	template<typename T>
	using tmat4 = Eigen::Matrix4<T>;
	using mat4 = Eigen::Matrix4f;

	template<int Cols>
	using vecn = Eigen::Vector<float, Cols>;

	using quat = Eigen::Quaternionf;
	using vec2 = Eigen::Vector2f;
	using vec3 = Eigen::Vector3f;
	using vec4 = Eigen::Vector4f;
	using rvec2 = Eigen::RowVector2f;
	using rvec3 = Eigen::RowVector3f;
	using rvec4 = Eigen::RowVector4f;
	using uvec2 = Eigen::Vector2<uint32_t>;
	using uvec3 = Eigen::Vector3<uint32_t>;
	using uvec4 = Eigen::Vector4<uint32_t>;
	using ruvec2 = Eigen::RowVector2<uint32_t>;
	using ruvec3 = Eigen::RowVector3<uint32_t>;
	using ruvec4 = Eigen::RowVector4<uint32_t>;

	using translation3 = Eigen::Translation3f;
	using affine3 = Eigen::Transform<float, 3, Eigen::Affine>;
	using angleaxis = Eigen::AngleAxisf;
	using scaling3 = Eigen::AlignedScaling3f;
}