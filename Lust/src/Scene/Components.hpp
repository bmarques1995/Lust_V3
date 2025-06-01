#pragma once

#include "LustDLLMacro.hpp"
#include <Eigen/Eigen>
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
		Eigen::Matrix4f Transform = Eigen::Matrix4f::Identity();

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const Eigen::Matrix4f& transform)
			: Transform(transform)
		{
		}

		operator Eigen::Matrix4f& ()
		{
			return Transform;
		}

		operator const Eigen::Matrix4f& () const
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
		SpriteRendererComponent(const Eigen::Vector4f& color)
		{
			ColorTexInfo.controllerInfo = Eigen::Vector4<uint32_t>(0, 0, 0, 0);
			ColorTexInfo.texCoordsEdges = Eigen::Vector4f(0.0f, 0.0f, 1.0f, 1.0f);
			ColorTexInfo.edgeColors = ColorCaster::CastFloatColor(color);
		}
		SpriteRendererComponent(const Eigen::Vector4f& color0, const Eigen::Vector4f& color1, const Eigen::Vector4f& color2, const Eigen::Vector4f& color3)
		{
			ColorTexInfo.controllerInfo = Eigen::Vector4<uint32_t>(0, 0, 0, 0);
			ColorTexInfo.texCoordsEdges = Eigen::Vector4f(0.0f, 0.0f, 1.0f, 1.0f);
			ColorTexInfo.edgeColors = ColorCaster::CastFloatColor(color0, color1, color2, color3);
		}

		void SetRenderComponents(const Eigen::Vector4<uint32_t>& components)
		{
			ColorTexInfo.controllerInfo = components;
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
