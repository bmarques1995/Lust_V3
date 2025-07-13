#include "TransformComponent.hpp"

void Lust::TransformComponent::UpdateTransform()
{
	quat qx(angleaxis(Rotation.x(), vec3::UnitX()));
	quat qy(angleaxis(Rotation.y(), vec3::UnitY()));
	quat qz(angleaxis(Rotation.z(), vec3::UnitZ()));

	// Combine in desired rotation order (e.g., ZYX)
	quat rotation = qz * qy * qx;
	scaling3 baseScale = scaling(Scale.x(), Scale.y(), Scale.z());
	affine3 transform = rotation * translation3(Position) * baseScale;
	Transform = transform.matrix().transpose();
}

Lust::TransformComponent::TransformComponent(const vec3& position, const vec3& rotation, const vec3& scale)
	: Position(position), Rotation(rotation), Scale(scale)
{
	UpdateTransform();
}

void Lust::TransformComponent::SetPosition(const vec3& position)
{
	Position = position;
	UpdateTransform();
}

void Lust::TransformComponent::SetRotation(const vec3& rotation)
{
	Rotation = rotation;
	UpdateTransform();
}

void Lust::TransformComponent::SetScale(const vec3& scale)
{
	Scale = scale;
	UpdateTransform();
}

Lust::vec3 Lust::TransformComponent::GetPosition() const
{
	return Position;
}

Lust::vec3 Lust::TransformComponent::GetRotation() const
{
	return Rotation;
}

Lust::vec3 Lust::TransformComponent::GetScale() const
{
	return Scale;
}

Lust::mat4 Lust::TransformComponent::GetTransform() const
{
	return Transform;
}
