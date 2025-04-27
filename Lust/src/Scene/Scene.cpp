#include "Scene.hpp"
#include "Components.hpp"
#include "Entity.hpp"

Lust::Scene::Scene()
{
	//entt::entity entity = m_Registry.create();
}

Lust::Scene::~Scene()
{
}

Lust::Entity Lust::Scene::CreateEntity()
{
	return { m_Registry.create(), this };
}

void Lust::Scene::OnUpdate(Timestep deltaTime)
{
	auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
	/*group.sort<TransformComponent>([](const TransformComponent& lhs, const TransformComponent& rhs) {
		return lhs.SortingLayer < rhs.SortingLayer;
	});*/
	for (auto entity : group)
	{
		auto& transform = group.get<TransformComponent>(entity);
		auto& sprite = group.get<SpriteRendererComponent>(entity);
		Lust::Renderer2D::DrawQuad(transform.Transform, sprite.ColorTexInfo);
	}
}
