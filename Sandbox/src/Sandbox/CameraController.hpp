#pragma once

#include <Components.hpp>

namespace Lust
{
	class CameraController : public ScriptableEntity
	{
	public:
		void OnCreate() override;
		void OnDestroy() override;
		void OnUpdate(Timestep ts) override;
	};
}
