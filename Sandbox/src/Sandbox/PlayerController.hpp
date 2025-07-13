#pragma once

#include <ScriptableEntity.hpp>

namespace Lust
{
	class PlayerController : public ScriptableEntity
	{
	public:
		void SetPlayer(Entity* entity);
	protected:
		void OnCreate() override;
		void OnDestroy() override;
		void OnUpdate(Timestep ts) override;
		void OnEvent(Event* e) override;
		void OnCommand() override;
	private:
		Entity* m_Player;
		float m_CameraTranslationSpeed = 200.0f;
	};
}