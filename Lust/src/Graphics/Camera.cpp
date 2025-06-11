#include "Camera.hpp"

Lust::Camera::Camera(const mat4& projectionMatrix, const mat4& viewMatrix) :
	m_ProjectionMatrix(projectionMatrix), m_ViewMatrix(viewMatrix)
{
}


const Lust::mat4& Lust::Camera::GetProjectionMatrix() const
{
	return m_ProjectionMatrix;
}

const Lust::mat4& Lust::Camera::GetViewMatrix() const
{
	return m_ViewMatrix;
}

void Lust::Camera::ResetProjectionMatrix(const mat4& projectionMatrix)
{
	m_ProjectionMatrix = projectionMatrix;
}

void Lust::Camera::ResetViewMatrix(const mat4& viewMatrix)
{
	m_ViewMatrix = viewMatrix;
}
