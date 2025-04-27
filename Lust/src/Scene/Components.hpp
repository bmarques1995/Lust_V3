#pragma once

#include "LustDLLMacro.hpp"
#include <Eigen/Eigen>
#include <ColorCaster.hpp>
#include <Renderer2D.hpp>

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
	};
}