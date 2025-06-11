#pragma once

#include "LustDLLMacro.hpp"
#include <MathComponents.hpp>
#include <ColorCaster.hpp>
#include "Renderer2D.hpp"
#include "Camera.hpp"
#include "ScriptableEntity.hpp"
#include "Timestep.hpp"
#include <functional>

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

	struct LUST_API SpriteRendererComponent
	{
		SSBOInstanceData ColorTexInfo;
		uint32_t DrawOrder = 0;

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
