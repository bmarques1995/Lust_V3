#include "OrthographicCamera.hpp"
#include "Operations.hpp"

Lust::OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top) :
	m_ProjectionMatrix(Ortho(left, right, bottom, top, 0.0f, 1.0f)), m_ViewMatrix(Eigen::Matrix4f::Identity())
{
	m_ViewProjectionMatrix = m_ViewMatrix * m_ProjectionMatrix;
}

void Lust::OrthographicCamera::SetProjection(float left, float right, float bottom, float top)
{
	m_ProjectionMatrix = Ortho(left, right, bottom, top, 0.0f, 1.0f);
	m_ViewProjectionMatrix = m_ViewMatrix * m_ProjectionMatrix;
}

void Lust::OrthographicCamera::SetPosition(const Eigen::Vector3f& position)
{
	m_CameraPosition = position;
	RecalculateViewMatrix();
}

void Lust::OrthographicCamera::SetRotation(float rotation)
{
	m_Rotation = rotation;
	RecalculateViewMatrix();
}

const Eigen::Vector3f& Lust::OrthographicCamera::GetPosition() const
{
	return m_CameraPosition;
}

float Lust::OrthographicCamera::GetRotation(float rotation) const
{
	return m_Rotation;
}

const Eigen::Matrix4f& Lust::OrthographicCamera::GetProjectionMatrix() const
{
	return m_ProjectionMatrix;
}

const Eigen::Matrix4f& Lust::OrthographicCamera::GetViewMatrix() const
{
	return m_ViewMatrix;
}

const Eigen::Matrix4f& Lust::OrthographicCamera::GetViewProjectionMatrix() const
{
	return m_ViewProjectionMatrix;
}

void Lust::OrthographicCamera::RecalculateViewMatrix()
{
	Eigen::Quaternionf q(Eigen::AngleAxisf(m_Rotation, Eigen::Vector3f(0.0f, 0.0f, 1.0f)));
	Eigen::Transform<float, 3, Eigen::Affine> transform = q * Eigen::Translation3f(m_CameraPosition);

	m_ViewMatrix = transform.matrix().transpose().inverse();
	m_ViewProjectionMatrix = m_ViewMatrix * m_ProjectionMatrix;
}
