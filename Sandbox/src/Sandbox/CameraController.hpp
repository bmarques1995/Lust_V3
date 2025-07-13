#pragma once

#include <Components.hpp>
#include <OrthographicCameraController.hpp>

namespace Lust
{
	class CameraController : public ScriptableEntity
	{
	public:
		void ResetCamera();
		
		void Follow(const InGameTransformComponent* transform);
	protected:
		void OnCreate() override;
		void OnDestroy() override;
		void OnUpdate(Timestep ts) override;
		void OnEvent(Event* e) override;
		void OnCommand() override;
	private:
		std::shared_ptr<OrthographicCameraController> m_CameraController;
		const InGameTransformComponent* m_Transform;
	};
}
