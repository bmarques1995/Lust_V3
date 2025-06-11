#include "OrthographicCamera.hpp"
#include "Operations.hpp"

Lust::OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top) :
	m_ProjectionMatrix(Ortho(left, right, bottom, top, 0.0f, 1.0f)), m_ViewMatrix(Lust::mat4::Identity()), m_Rotation(0.0f)
{
	m_ViewProjectionMatrix = m_ViewMatrix * m_ProjectionMatrix;
}

void Lust::OrthographicCamera::SetProjection(float left, float right, float bottom, float top)
{
	m_ProjectionMatrix = Ortho(left, right, bottom, top, 0.0f, 1.0f);
	m_ViewProjectionMatrix = m_ViewMatrix * m_ProjectionMatrix;
}

void Lust::OrthographicCamera::SetPosition(const vec3& position)
{
	m_CameraPosition = position;
	RecalculateViewMatrix();
}

void Lust::OrthographicCamera::SetRotation(float rotation)
{
	m_Rotation = rotation;
	RecalculateViewMatrix();
}

const Lust::vec3& Lust::OrthographicCamera::GetPosition() const
{
	return m_CameraPosition;
}

float Lust::OrthographicCamera::GetRotation(float rotation) const
{
	return m_Rotation;
}

void Lust::OrthographicCamera::SetPositionAndRotation(const vec3& position, float rotation)
{
	m_CameraPosition = position;
	m_Rotation = rotation;
	RecalculateViewMatrix();
}

const Lust::mat4& Lust::OrthographicCamera::GetProjectionMatrix() const
{
	return m_ProjectionMatrix;
}

const Lust::mat4& Lust::OrthographicCamera::GetViewMatrix() const
{
	return m_ViewMatrix;
}

const Lust::mat4& Lust::OrthographicCamera::GetViewProjectionMatrix() const
{
	return m_ViewProjectionMatrix;
}

void Lust::OrthographicCamera::RecalculateViewMatrix()
{
	quat q(angleaxis(m_Rotation, vec3(0.0f, 0.0f, 1.0f)));
	affine3 transform = q * translation3(m_CameraPosition);

	m_ViewMatrix = transform.matrix().transpose().inverse();
	m_ViewProjectionMatrix = m_ViewMatrix * m_ProjectionMatrix;
}
