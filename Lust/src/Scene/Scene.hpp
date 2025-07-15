#pragma once

#include <entt/entt.hpp>
#include "LustDLLMacro.hpp"
#include "Timestep.hpp"
#include "Event.hpp"
#include <memory>

namespace Lust
{
	struct Entity;

	class LUST_API Scene
	{
		friend class Entity;
	public:

		void DestroyEntity(Entity* entity);
		Scene();
		virtual ~Scene();

		void InstantiateEntities();

		Entity CreateEntity();
		void OnUpdate(Timestep deltaTime);
		void OnEvent(Event* e);
		void OnCommand();
	private:
		entt::registry m_Registry;
	};
}