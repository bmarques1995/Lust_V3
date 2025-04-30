#include "Camera.hpp"

Lust::Camera::Camera(const Eigen::Matrix4f& projectionMatrix, const Eigen::Matrix4f& viewMatrix) :
	m_ProjectionMatrix(projectionMatrix), m_ViewMatrix(viewMatrix)
{
}


const Eigen::Matrix4f& Lust::Camera::GetProjectionMatrix() const
{
	return m_ProjectionMatrix;
}

const Eigen::Matrix4f& Lust::Camera::GetViewMatrix() const
{
	return m_ViewMatrix;
}

void Lust::Camera::ResetProjectionMatrix(const Eigen::Matrix4f& projectionMatrix)
{
	m_ProjectionMatrix = projectionMatrix;
}

void Lust::Camera::ResetViewMatrix(const Eigen::Matrix4f& viewMatrix)
{
	m_ViewMatrix = viewMatrix;
}
