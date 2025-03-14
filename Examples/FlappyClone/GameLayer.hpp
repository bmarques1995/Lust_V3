#pragma once

#include <Layer.hpp>
#include <OrthographicCameraController.hpp>
#include <ApplicationEvent.hpp>
#include "Level.hpp"

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

	Level m_Level;

	Eigen::Vector3f m_SquareColor = { 0.2f, 0.3f, 0.8f };
};