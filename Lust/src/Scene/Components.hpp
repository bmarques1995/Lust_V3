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
		mat4 Transform = mat4::Identity();

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const mat4& transform)
			: Transform(transform)
		{
		}

		operator mat4& ()
		{
			return Transform;
		}

		operator const mat4& () const
		{
			return Transform;
		}
	};

	struct LUST_API InGameTransformComponent
	{
	private:
		vec3 Position = { 0.0f, 0.0f, 0.0f };
		vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		vec3 Scale = { 1.0f, 1.0f, 1.0f };
		mat4 Transform = mat4::Identity();

		void UpdateTransform()
		{
			quat qx(angleaxis(Rotation.x(), vec3::UnitX()));
			quat qy(angleaxis(Rotation.y(), vec3::UnitY()));
			quat qz(angleaxis(Rotation.z(), vec3::UnitZ()));

			// Combine in desired rotation order (e.g., ZYX)
			quat rotation = qz * qy * qx;
			scaling3 baseScale = scaling(Scale.x(), Scale.y(), Scale.z());
			affine3 transform = rotation * baseScale * translation3(Position);
			Transform = transform.matrix().transpose();
		}

	public:

		InGameTransformComponent() = default;
		InGameTransformComponent(const InGameTransformComponent&) = default;
		InGameTransformComponent(const vec3& position, const vec3& rotation, const vec3& scale)
			: Position(position), Rotation(rotation), Scale(scale)
		{
		}

		vec3 GetPosition() const { return Position; }
		vec3 GetRotation() const { return Rotation; }
		vec3 GetScale() const { return Scale; }
		mat4 GetTransform() const { return Transform; }

		void SetPosition(const vec3& position)
		{ 
			Position = position;
			UpdateTransform();
		}
		
		void SetRotation(const vec3& rotation)
		{ 
			Rotation = rotation;
			UpdateTransform();
		}
		void SetScale(const vec3& scale)
		{ 
			Scale = scale;
			UpdateTransform();
		}
	};

	struct LUST_API SpriteRendererComponent
	{
		SSBOInstanceData ColorTexInfo;
		uint32_t DrawOrder = 0;
		std::shared_ptr<Sprite2DSheet> SpriteSheet;

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const vec4& color)
		{
			ColorTexInfo.controllerInfo = uvec4(0, 0, 0, 0);
			ColorTexInfo.texCoordsEdges = vec4(0.0f, 0.0f, 1.0f, 1.0f);
			ColorTexInfo.edgeColors = ColorCaster::CastFloatColor(color);
		}

		SpriteRendererComponent(const vec4& color0, const vec4& color1, const vec4& color2, const vec4& color3)
		{
			ColorTexInfo.controllerInfo = uvec4(0, 0, 0, 0);
			ColorTexInfo.texCoordsEdges = vec4(0.0f, 0.0f, 1.0f, 1.0f);
			ColorTexInfo.edgeColors = ColorCaster::CastFloatColor(color0, color1, color2, color3);
		}

		void SetDrawOrder(uint32_t drawOrder)
		{
			DrawOrder = drawOrder;
		}

		void SetTexture(uint32_t textureIndex, uint32_t samplerIndex)
		{
			ColorTexInfo.controllerInfo = uvec4(textureIndex, samplerIndex, 0, 0);
		}

		void AddSpriteSheet(std::shared_ptr<Sprite2DSheet> spriteSheet)
		{
			SpriteSheet = spriteSheet;
		}

		void AddSpriteSheet(std::shared_ptr<Texture2D> texture, uint32_t spriteWidth, uint32_t spriteHeight)
		{
			SpriteSheet.reset(new Sprite2DSheet(texture, spriteWidth, spriteHeight));
		}

		void SetSprite(uint32_t xIndex, uint32_t yIndex)
		{
			ColorTexInfo.texCoordsEdges = SpriteSheet->GetSpriteUV(xIndex, yIndex);
		}

		void SetUV(const vec4& formattedUV)
		{
			ColorTexInfo.texCoordsEdges = formattedUV;
		}
	};

	struct CameraComponent
	{
		Camera CameraElement;
		bool Primary = true;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
		CameraComponent(const Lust::Camera& camera)
			: CameraElement(camera)
		{
		}
	};

	struct NativeScriptComponent
	{
		ScriptableEntity* Instance = nullptr;

		ScriptableEntity* (*InstantiateScript)();
		void (*DestroyScript)(NativeScriptComponent*);

		template<typename T>
		inline void Bind()
		{
			InstantiateScript = []() -> ScriptableEntity* { return new T(); } ;
			DestroyScript = [](NativeScriptComponent* nsc)
			{ 
				delete nsc->Instance;
				nsc->Instance = nullptr;
			};
		}

		template<typename T>
		inline T* GetInnerComponent()
		{
			return Instance->GetComponent<T>();
		}
	};
}
