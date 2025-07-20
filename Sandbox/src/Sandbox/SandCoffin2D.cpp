#include "SandCoffin2D.hpp"
#include "MathBuffer.hpp"
#include <Application.hpp>
#include <imgui/imgui.h>
#include <RenderCommand.hpp>
#include <Renderer2D.hpp>
#include <chrono>
#include <iostream>
#include <cstdio>
#include "Instrumentator.hpp"
#include "CameraController.hpp"
#include "PlayerController.hpp"
#include <Components.hpp>
#include <Operations.hpp>

Lust::SandCoffin2D::SandCoffin2D() :
	Layer("SandCoffin2D")
{

}

Lust::SandCoffin2D::~SandCoffin2D()
{

}

void Lust::SandCoffin2D::OnAttach()
{
	//auto window = Application::GetInstance()->GetWindow();
	auto context = Application::GetInstance()->GetContext();
	
	SamplerInfo samplerInfoController(SamplerFilter::NEAREST, AnisotropicFactor::FACTOR_4, AddressMode::REPEAT, ComparisonPassMode::ALWAYS);
	m_Renderer2DSampler.reset(Sampler::Instantiate(context, samplerInfoController));
	m_Renderer2DTexture.reset(Texture2D::Instantiate(context, "./assets/textures/sample.png"));
	//m_SpriteSheetTexture.reset(Texture2D::Instantiate(context, "./assets/textures/rpg_pack.png"));
	//m_SpriteSheet.reset(new Sprite2DSheet(m_SpriteSheetTexture, 128, 128));
	
	//m_SampleSocket.reset(Sockets::Instantiate());
	//m_SampleSocket->OpenConnection("127.0.0.1", 8300, SocketType::TCP, false);
	m_SampleScene.reset(new Scene());
	{
		auto square = m_SampleScene->CreateEntity();
		//pos, rot, scale
		square.AddComponent<TransformComponent>(vec3(0.0f, 0.0f, .0f), vec3(0.0f, 0.0f, 0.0f), vec3(150.0f, 130.0f, 1.0f));
		square.AddComponent<SpriteRendererComponent>(
			Eigen::Vector4f(1.0f, 1.0f, 1.0f, 1.0f)
		);

		square.AddComponent<NativeScriptComponent>()->Bind<PlayerController>();
		m_SquareEntity = square;
	}

	{
		/*auto square = m_SampleScene->CreateEntity();
		square.AddComponent<TransformComponent>(vec3(0.0f, 0.0f, .0f), vec3(0.0f, 0.0f, 0.0f), vec3(15000.0f, 13000.0f, 1.0f));
		square.AddComponent<SpriteRendererComponent>(
			Eigen::Vector4f(1.0f, 1.0f, 1.0f, 1.0f)
		);

		m_SquareEntity2 = square;*/
	}
	std::shared_ptr<TilemapComponent> tilemap;
	TilemapComponent::LoadTilemapFromJson("./assets/texture_map/scenario.json", &m_SpriteSheetTexture, &m_SpriteSheet, &tilemap);

	auto spriteRenderer = m_SquareEntity.GetComponent<SpriteRendererComponent>();
	spriteRenderer->DrawOrder = 3;
	spriteRenderer->SetTexture(2, 1);
	spriteRenderer->AddSpriteSheet(m_SpriteSheet);
	spriteRenderer->SetSprite(6, 7);

	m_TilemapEntity = m_SampleScene->CreateEntity();
	
	//uint32_t width, uint32_t height, Sprite2DSheet spriteSheet, const uint32_t textureIndex = 1, const uint32_t samplerIndex = 1, const vec2& initialPos = { 0.0f, 0.0f }
	auto tilemapComponent = m_TilemapEntity.AddComponent<TilemapComponent>(tilemap.get());
	//vec2 initialPos = { -448.0f, -448.0f };
	//tilemapComponent->SetInitialPosition(initialPos);
	tilemapComponent->SetTextureIndex(2);
	tilemapComponent->SetSamplerIndex(1);

	m_CameraEntity = m_SampleScene->CreateEntity();
	m_CameraEntity.AddComponent<NativeScriptComponent>()->Bind<CameraController>();
	m_SampleScene->InstantiateEntities();
	auto cameraScript = m_CameraEntity.GetComponent<NativeScriptComponent>()->GetImplementedScript<CameraController>();
	cameraScript->Follow(&m_SquareEntity);

	auto script = m_SquareEntity.GetComponent<NativeScriptComponent>()->GetImplementedScript<PlayerController>();
	script->SetPlayer(&m_SquareEntity);
	//auto cameraController = m_CameraEntity.GetComponent<CameraController>();
	//cameraController->Follow();

	Renderer2D::UploadTexture2D(m_Renderer2DTexture);
	Renderer2D::UploadTexture2D(m_SpriteSheetTexture, 2);
	Renderer2D::UploadSampler(m_Renderer2DSampler);
}

void Lust::SandCoffin2D::OnDetach()
{
	m_SampleScene->DestroyEntity(&m_TilemapEntity);
	m_SampleScene->DestroyEntity(&m_SquareEntity);
	m_SampleScene->DestroyEntity(&m_CameraEntity);
	m_SpriteSheetTexture.reset();
	m_Renderer2DTexture.reset();
	m_Renderer2DSampler.reset();
	//m_SampleSocket->CloseConnection();
}

void Lust::SandCoffin2D::OnUpdate(Timestep ts)
{
	Eigen::Vector2f texSize;
	{
	}
	{
		//m_SampleSocket->SendData("Hello from Lust Engine\n");
		//m_SampleSocket->Update();
		//Renderer2D::BeginScene(m_CameraController->GetCamera());
		m_SampleScene->OnUpdate(ts);
		//Renderer2D::EndScene();
	}
	{
		//texSize = Eigen::Vector2f((float)m_Renderer2DTexture->GetWidth() * 25, (float)m_Renderer2DTexture->GetHeight() * 25);
	}
	{
		//Eigen::Vector2<uint32_t> atlasIndex = Eigen::Vector2<uint32_t>(7, 6);
		//Eigen::Vector2f textureSize = Eigen::Vector2f((float)m_SpriteSheetTexture->GetWidth(), (float)m_SpriteSheetTexture->GetHeight());
		//Eigen::Vector2f spriteSize = Eigen::Vector2f(128.0f, 128.0f);
		//Eigen::Vector4<uint32_t> controllerInfo = Eigen::Vector4<uint32_t>(1, 1, 0, 0);
		//Eigen::Vector4<uint32_t> controllerInfo2 = Eigen::Vector4<uint32_t>(2, 0, 0, 0);
		//Eigen::Vector4f texCoordsEdges = Eigen::Vector4f(0.0f, 0.0f, 1.0f, 1.0f);
		//Renderer2D::BeginScene(m_CameraController->GetCamera());
		////Eigen::Vector3f m_Renderer2DColor = { .2f, 0.3f, 0.8f };
		//Renderer2D::DrawQuad(Eigen::Vector3f(0.0f, 0.0f, 0.0f), Eigen::Vector2f(150.0f, 150.0f), { Eigen::Vector4<uint32_t>(0, 0, 0, 0), texCoordsEdges, m_Renderer2DColor });
		//Renderer2D::DrawQuad(Eigen::Vector3f(450.0f, 200.0f, 0.0f), Eigen::Vector2f(100.0f, 130.0f), { Eigen::Vector4<uint32_t>(0, 0, 0, 0), texCoordsEdges, m_Renderer2DColor2 });
		////Modify system, update texture and sampler once and only rebind if the texture or sampler changes
		//Renderer2D::DrawQuad(Eigen::Vector3f(0.0f, 0.0f, 0.2f), texSize, { controllerInfo, texCoordsEdges * 12.0f, Eigen::Vector4<uint32_t>(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff) });
		//Renderer2D::DrawQuad(Eigen::Vector3f(150.0f, 140.0f, 0.0f), Eigen::Vector2f(static_cast<float>(m_SpriteSheet->GetSpriteWidth()), static_cast<float>(m_SpriteSheet->GetSpriteHeight() * 2)),
		//	{ controllerInfo2, m_SpriteSheet->GetSpriteUV(4, 10, 1, 2), Eigen::Vector4<uint32_t>(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff)});
		//Renderer2D::EndScene();
	}
}

void Lust::SandCoffin2D::OnImGuiRender()
{
	ImGui::Begin("Color Editor");
	ImGui::ColorEdit3("Square Color", &m_Renderer2DColorFloat(0));
	m_Renderer2DColor = CastFloatColor(m_Renderer2DColorFloat);
	if (ImGui::Button("Reset camera"))
	{
		m_SampleScene->OnCommand();
	}
	for (auto& profile: m_ProfileResults)
	{
		char label[100];
		snprintf(label, sizeof(label), "%.3fms %s", profile.Time, profile.Name);

		ImGui::Text(label);
	}
	m_ProfileResults.clear();
	ImGui::End();
}

void Lust::SandCoffin2D::OnEvent(Event& event)
{
	m_SampleScene->OnEvent(&event);
	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<WindowResizedEvent>(std::bind(&SandCoffin2D::OnWindowResize, this, std::placeholders::_1), true);
}

Eigen::Vector4<uint32_t> Lust::SandCoffin2D::CastFloatColor(Eigen::Vector3f color)
{
	uint8_t r = (uint8_t)(color(0) * 255.0f);
	uint8_t g = (uint8_t)(color(1) * 255.0f);
	uint8_t b = (uint8_t)(color(2) * 255.0f);
	uint32_t pixel = (r << 24) | (g << 16) | (b << 8) | 0xff;
	return Eigen::Vector4<uint32_t>(pixel, pixel, pixel, pixel);
}

bool Lust::SandCoffin2D::OnWindowResize(WindowResizedEvent& e)
{
	return false;
}
