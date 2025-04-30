#pragma once

#include "LustDLLMacro.hpp"
#include <entt/entt.hpp>
#include "Scene.hpp"
#include "Console.hpp"
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
		Entity() = default;
		Entity(entt::entity handle, Scene* scene);
		Entity(const Entity&) = default;

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

		template<typename T>
		T* GetComponent()
		{
			Console::CoreAssert(HasComponent<T>(), "Entity does not have this component");
			return m_Scene->m_Registry.try_get<T>(m_EntityHandle);
		}

		template<typename T>
		void RemoveComponent()
		{
			Console::CoreAssert(HasComponent<T>(), "Entity does not have this component");
			m_Scene->m_Registry.remove<T>(m_EntityHandle);
		}
	private:
		entt::entity m_EntityHandle {entt::null} ;
		Scene* m_Scene = nullptr;
	};
}