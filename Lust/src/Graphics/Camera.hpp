#pragma once

#include "LustDLLMacro.hpp"
#include <Eigen/Eigen>

namespace Lust
{
	class LUST_API Camera
	{
	public:
		Camera(const Eigen::Matrix4f& projectionMatrix, const Eigen::Matrix4f& viewMatrix);

		const Eigen::Matrix4f& GetProjectionMatrix() const;
		const Eigen::Matrix4f& GetViewMatrix() const;

		void ResetProjectionMatrix(const Eigen::Matrix4f& projectionMatrix);
		void ResetViewMatrix(const Eigen::Matrix4f& viewMatrix);
	private:
		Eigen::Matrix4f m_ProjectionMatrix = Eigen::Matrix4f::Identity();
		Eigen::Matrix4f m_ViewMatrix = Eigen::Matrix4f::Identity();
	};
}