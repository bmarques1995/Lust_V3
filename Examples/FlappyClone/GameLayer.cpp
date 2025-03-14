#include "GameLayer.hpp"
#include "Application.hpp"
#include <Renderer2D.hpp>
#include <imgui/imgui.h>

GameLayer::GameLayer()
{
	auto window = Lust::Application::GetInstance()->GetWindow();
	auto context = Lust::Application::GetInstance()->GetEditableContext();
	context->SetClearColor(0.3f, 0.3f, 0.3f, 1.0f);

	CreateCamera(window->GetWidth(), window->GetHeight());
}

GameLayer::~GameLayer()
{
}

void GameLayer::OnAttach()
{
	m_Level.Init();
}

void GameLayer::OnDetach()
{
}

void GameLayer::OnUpdate(Lust::Timestep ts)
{
	m_Level.OnUpdate(ts);

	const auto& playerPos = m_Level.GetPlayer().GetPosition();
	m_Camera->SetPosition(Eigen::Vector3f{ playerPos.x(), 0.0f, 0.0f });
	Lust::Renderer2D::BeginScene(m_Camera.get());
	m_Level.OnRender();
	Lust::Renderer2D::EndScene();
}

void GameLayer::OnImGuiRender()
{
	ImGui::Begin("Settings");
	ImGui::ColorEdit3("Square Color", m_SquareColor.data());
	m_Level.OnImGuiRender();
	ImGui::End();
}

void GameLayer::OnEvent(Lust::Event& event)
{
	Lust::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<Lust::WindowResizedEvent>(std::bind(&GameLayer::OnWindowResize, this, std::placeholders::_1), false);
}

bool GameLayer::OnWindowResize(Lust::WindowResizedEvent& e)
{
	CreateCamera(e.GetWidth(), e.GetHeight());
	return false;
}

void GameLayer::CreateCamera(uint32_t width, uint32_t height)
{
	float aspectRatio = (float)width / (float)height;
	float camWidth = 10.0f;
	float bottom = -camWidth;
	float top = camWidth;
	float left = -camWidth * aspectRatio;
	float right = camWidth * aspectRatio;
	m_Camera = std::make_unique<Lust::OrthographicCamera>(left, right, bottom, top);
	//m_CameraController = std::make_unique<Lust::OrthographicCameraController>(m_Camera);

}
