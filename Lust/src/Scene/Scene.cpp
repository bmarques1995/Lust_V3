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
	Camera* mainCamera = nullptr;
	{
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			mainCamera = &view.get<CameraComponent>(entity).CameraElement;
			break;
		}
	}

	if (mainCamera)
	{
		auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		group.sort<SpriteRendererComponent>([](const SpriteRendererComponent& lhs, const SpriteRendererComponent& rhs)
			{
				return lhs.DrawOrder < rhs.DrawOrder;
			});
		Renderer2D::BeginScene(mainCamera);
		for (auto entity : group)
		{
			auto& transform = group.get<TransformComponent>(entity);
			auto& sprite = group.get<SpriteRendererComponent>(entity);
			Lust::Renderer2D::DrawQuad(transform.Transform, sprite.ColorTexInfo);
		}
		Renderer2D::EndScene();
	}
}
