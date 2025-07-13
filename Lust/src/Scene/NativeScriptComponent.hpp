#pragma once

#include "LustDLLMacro.hpp"
#include "ScriptableEntity.hpp"
#include <functional>

namespace Lust
{
	struct LUST_API NativeScriptComponent
	{
		ScriptableEntity* Instance = nullptr;

		ScriptableEntity* (*InstantiateScript)();
		void (*DestroyScript)(NativeScriptComponent*);

		template<typename T>
		inline void Bind()
		{
			InstantiateScript = []() -> ScriptableEntity* { return new T(); } ;
			DestroyScript = [](NativeScriptComponent* nsc)
			{ 
				delete nsc->Instance;
				nsc->Instance = nullptr;
			};
		}

		template<typename T>
		inline T* GetImplementedScript()
		{
			return static_cast<T*>(Instance);
		}

		template<typename T>
		inline T* GetInnerComponent()
		{
			return Instance->GetComponent<T>();
		}
	};
}
