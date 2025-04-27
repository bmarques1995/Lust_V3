#pragma once

#include <entt/entt.hpp>
#include "LustDLLMacro.hpp"
#include "Timestep.hpp"
#include <memory>

namespace Lust
{
	struct Entity;

	class LUST_API Scene
	{
		friend class Entity;
	public:
		Scene();
		virtual ~Scene();

		Entity CreateEntity();
		void OnUpdate(Timestep deltaTime);
	private:
		entt::registry m_Registry;
	};
}