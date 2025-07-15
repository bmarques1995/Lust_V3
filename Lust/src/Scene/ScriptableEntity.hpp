#pragma once

#include "Entity.hpp"
#include "Event.hpp"
#include "LustDLLMacro.hpp"

namespace Lust
{
	class ScriptableEntity
	{
	public:
		virtual ~ScriptableEntity() {}

		template<typename T>
		T* GetComponent()
		{
			return m_Entity.GetComponent<T>();
		}

		template<typename T, typename... Args>
		inline T* AddComponent(Args &&... args)
		{
			return m_Entity.AddComponent<T>(std::forward<Args>(args)...);
		}

	protected:
		virtual void OnCreate() {}
		virtual void OnDestroy() {}
		virtual void OnUpdate(Timestep ts) {}
		virtual void OnEvent(Event* e) {}
		virtual void OnCommand() {}

	private:
		Entity m_Entity;
		friend class Scene;
	};
}
