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

void Lust::Scene::InstantiateEntities()
{
	m_Registry.view<NativeScriptComponent>().each([=](entt::entity entity, NativeScriptComponent& nsc)
	{
		if (!nsc.Instance)
		{
			nsc.Instance = nsc.InstantiateScript();
			nsc.Instance->m_Entity = { entity, this };
			nsc.Instance->OnCreate();
		}
	});
}


Lust::Entity Lust::Scene::CreateEntity()
{
	return { m_Registry.create(), this };
}

void Lust::Scene::OnUpdate(Timestep deltaTime)
{
	{
		m_Registry.view<NativeScriptComponent>().each([=](entt::entity entity, NativeScriptComponent& nsc)
		{
			if (!nsc.Instance)
			{
				nsc.Instance = nsc.InstantiateScript();
				nsc.Instance->m_Entity = { entity, this };
				nsc.Instance->OnCreate();
			}
			nsc.Instance->OnUpdate(deltaTime);
		});
	}

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
		auto tileGroup = m_Registry.group<TilemapComponent>();
		Renderer2D::BeginScene(mainCamera);
		for (auto entity : tileGroup)
		{
			auto tilemap = tileGroup.get<TilemapComponent>(entity);
			SSBOInstanceData instanceData;
			for (size_t i = 0; i < tilemap.m_Width * tilemap.m_Height; i++)
			{
				uint32_t xIndex = i % tilemap.m_Width;
				uint32_t yIndex = i / tilemap.m_Width;
				auto& bind = tilemap.m_TilemapBinds[i];
				vec4 spriteCoords = tilemap.m_SpriteSheet->GetSpriteUV(bind.XIndex, bind.YIndex);
				float spriteWidth = tilemap.m_SpriteSheet->GetSpriteWidth();
				float spriteHeight = tilemap.m_SpriteSheet->GetSpriteHeight();
				instanceData.texCoordsEdges = spriteCoords;
				instanceData.controllerInfo = uvec4(tilemap.m_TextureIndex, tilemap.m_SamplerIndex, 0, 0);
				instanceData.edgeColors = uvec4(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
				vec2 pos = tilemap.m_InitialPos + vec2(xIndex * spriteWidth, yIndex * spriteHeight);
				vec2 size = { spriteWidth, spriteHeight };
				Lust::Renderer2D::DrawQuad(pos, size, instanceData);
			}
		}
		for (auto entity : group)
		{
			auto& transform = group.get<TransformComponent>(entity);
			auto& sprite = group.get<SpriteRendererComponent>(entity);
			Lust::Renderer2D::DrawQuad(transform.Transform, sprite.ColorTexInfo);
		}
		Renderer2D::EndScene();
	}
}

void Lust::Scene::OnEvent(Event* e)
{
	m_Registry.view<NativeScriptComponent>().each([=](NativeScriptComponent& nsc)
	{
		if (nsc.Instance)
		{
			nsc.Instance->OnEvent(e);
		}	

	});
	
}

void Lust::Scene::OnCommand()
{
	m_Registry.view<NativeScriptComponent>().each([=](NativeScriptComponent& nsc)
	{
		if (nsc.Instance)
		{
			nsc.Instance->OnCommand();
		}
	});
}
