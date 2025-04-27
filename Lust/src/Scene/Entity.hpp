#pragma once

#include "LustDLLMacro.hpp"
#include <entt/entt.hpp>
#include "Scene.hpp"
#include <memory>
#include <utility>

namespace Lust
{
	/**
	* @brief %Entity class
	* @details %Entity class for the Lust engine
	*/
	class LUST_API Entity
	{
	public:
		Entity(entt::entity handle, Scene* scene);
		
		template<typename T>
		bool HasComponent()
		{
			return m_Scene->m_Registry.all_of<T>(m_EntityHandle);
		}

		template<typename T, typename... Args>
		void AddComponent(Args &&... args)
		{
			m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
		}
	private:
		entt::entity m_EntityHandle;
		Scene* m_Scene;
	};
}