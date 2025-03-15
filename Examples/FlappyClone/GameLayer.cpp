#include "GameLayer.hpp"
#include "Application.hpp"
#include <Renderer2D.hpp>
#include <imgui/imgui.h>
#include <Application.hpp>

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
	m_Time = 0.0f;
}

void GameLayer::OnDetach()
{
}

void GameLayer::OnUpdate(Lust::Timestep ts)
{
	m_Time += ts;

	if ((int)(m_Time * 10.0f) % 8 > 4)
		m_Blink = !m_Blink;

	if (m_Level.IsGameOver())
		m_State = GameState::GameOver;

	switch (m_State)
	{
		case GameState::Play:
		{
			m_Level.OnUpdate(ts);
			break;
		}
	}

	const auto& playerPos = m_Level.GetPlayer().GetPosition();
	m_Camera->SetPosition(Eigen::Vector3f{ playerPos.x(), playerPos.y(), 0.0f });
	Lust::Renderer2D::BeginScene(m_Camera.get());
	m_Level.OnRender();
	Lust::Renderer2D::EndScene();
}

void GameLayer::OnImGuiRender()
{
	switch (m_State)
	{
		case GameState::Play:
		{
			uint32_t playerScore = m_Level.GetPlayer().GetScore();
			ImGui::Text("Score: %u", playerScore);
			break; 
		}
		case GameState::MainMenu:
		{
			if(ImGui::Button("Click to Play!"))
				m_State = GameState::Play;
			ImGui::Text("To move the ship, press space or\nA(xbox) or X(playstation)");
			break;
		}
		case GameState::GameOver:
		{
			
			ImGui::Text("Game Over!");
			ImGui::Text("Score: %u", m_Level.GetPlayer().GetScore());

			if (ImGui::Button("Click to Return!"))
			{
				m_Level.Reset();
				m_State = GameState::MainMenu;
			}
			break;
		}
	}
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
