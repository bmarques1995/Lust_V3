#pragma once

#include <Layer.hpp>
#include <OrthographicCameraController.hpp>
#include <ApplicationEvent.hpp>
#include "Level.hpp"
#include <imgui/imgui.h>

class GameLayer : public Lust::Layer
{
public:
	GameLayer();
	~GameLayer();

	void OnAttach() override;
	void OnDetach() override;

	void OnUpdate(Lust::Timestep ts) override;

	void OnImGuiRender() override;

	void OnEvent(Lust::Event& event) override;

	bool OnWindowResize(Lust::WindowResizedEvent& e);
private:

	void CreateCamera(uint32_t width, uint32_t height);

	std::unique_ptr<Lust::OrthographicCamera> m_Camera;
	ImFont* m_Font;
	Level m_Level;
	bool m_Blink = false;
	float m_Time;

	Lust::vec3 m_SquareColor = { 0.2f, 0.3f, 0.8f };

	enum class GameState
	{
		Play = 0, MainMenu = 1, GameOver = 2
	};

	GameState m_State = GameState::MainMenu;
};