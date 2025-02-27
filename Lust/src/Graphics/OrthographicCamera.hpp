#pragma once

#include <Eigen/Eigen>
#include "LustDLLMacro.hpp"

namespace Lust
{
	class LUST_API OrthographicCamera
	{
	public:
		OrthographicCamera(float left, float right, float bottom, float top);
	
		void SetProjection(float left, float right, float bottom, float top);

		void SetPosition(const Eigen::Vector3f& position);
		void SetRotation(float rotation);

		const Eigen::Vector3f& GetPosition() const;
		float GetRotation(float rotation) const;

		void SetPositionAndRotation(const Eigen::Vector3f& position, float rotation);

		const Eigen::Matrix4f& GetProjectionMatrix() const;
		const Eigen::Matrix4f& GetViewMatrix() const;
		const Eigen::Matrix4f& GetViewProjectionMatrix() const;

	private:
		
		void RecalculateViewMatrix();

		Eigen::Matrix4f m_ProjectionMatrix;
		Eigen::Matrix4f m_ViewMatrix;
		Eigen::Matrix4f m_ViewProjectionMatrix;

		Eigen::Vector3f m_CameraPosition;
		float m_Rotation;
	};
}