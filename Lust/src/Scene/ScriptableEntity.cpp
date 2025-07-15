#include "ScriptableEntity.hpp"
#include "Components.hpp"

void LustInternal_DestroyEntity(Lust::Entity* entity)
{
	while (entity->HasComponent<Lust::NativeScriptComponent>())
	{
		entity->GetComponent<Lust::NativeScriptComponent>()->Instance->OnDestroy();
		entity->RemoveComponent<Lust::NativeScriptComponent>();
	}
	entity->RemoveAllComponents<Lust::TilemapComponent>();
	entity->RemoveAllComponents<Lust::CameraComponent>();
	entity->RemoveAllComponents<Lust::TransformComponent>();
	entity->RemoveAllComponents<Lust::SpriteRendererComponent>();
}
