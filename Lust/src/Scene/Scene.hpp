#pragma once

#include <entt/entt.hpp>

namespace Lust
{
	class Scene
	{
	public:
		Scene();
		virtual ~Scene();

	private:
		entt::registry m_Registry;
	};
}