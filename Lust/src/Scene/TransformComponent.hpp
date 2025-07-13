#pragma once

#include "LustDLLMacro.hpp"
#include <MathComponents.hpp>
#include <ColorCaster.hpp>
#include "Renderer2D.hpp"
#include "Camera.hpp"
#include "ScriptableEntity.hpp"
#include "Timestep.hpp"
#include <functional>
#include "SpriteSheet.hpp"
#include <MathUtils.hpp>

namespace Lust
{
	struct LUST_API TransformComponent
	{
		friend class Scene;
	private:
		vec3 Position = { 0.0f, 0.0f, 0.0f };
		vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		vec3 Scale = { 1.0f, 1.0f, 1.0f };
		mat4 Transform = mat4::Identity();

		void UpdateTransform();

	public:

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const vec3& position, const vec3& rotation, const vec3& scale);

		vec3 GetPosition() const;
		vec3 GetRotation() const;
		vec3 GetScale() const;
		mat4 GetTransform() const;

		void SetPosition(const vec3& position);
		void SetRotation(const vec3& rotation);
		void SetScale(const vec3& scale);
	};
}
