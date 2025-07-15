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
		inline bool HasComponent() const
		{
			return m_Scene->m_Registry.all_of<T>(m_EntityHandle);
		}

		template<typename T, typename... Args>
		inline T* AddComponent(Args &&... args)
		{
			Console::CoreAssert(!this->HasComponent<T>(), "Entity already has component!");
			m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
			return this->GetComponent<T>();
		}

		template<typename T>
		inline T* GetComponent()
		{
			Console::CoreAssert(this->HasComponent<T>(), "Entity does not have this component");
			return m_Scene->m_Registry.try_get<T>(m_EntityHandle);
		}

		template<typename T>
		inline const T* GetConstComponent() const
		{
			Console::CoreAssert(this->HasComponent<T>(), "Entity does not have this component");
			return m_Scene->m_Registry.try_get<T>(m_EntityHandle);
		}

		template<typename T>
		inline void RemoveComponent()
		{
			Console::CoreAssert(this->HasComponent<T>(), "Entity does not have this component");
			m_Scene->m_Registry.remove<T>(m_EntityHandle);
		}

		template<typename T>
		inline void RemoveAllComponents()
		{
			while (HasComponent<T>())
			{
				m_Scene->m_Registry.remove<T>(m_EntityHandle);
			}
		}
	private:
		entt::entity m_EntityHandle {entt::null} ;
		Scene* m_Scene = nullptr;
	};
}