#pragma once

#include "LustDLLMacro.hpp"
#include "Camera.hpp"

namespace Lust
{
	struct LUST_API CameraComponent
	{
		Camera CameraElement;
		bool Primary = true;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
		CameraComponent(const Lust::Camera& camera);
	};
}
