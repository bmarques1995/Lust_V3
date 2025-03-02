#include "SandCoffin2D.hpp"
#include <Application.hpp>
#include <imgui/imgui.h>
#include <RenderCommand.hpp>
#include <Renderer2D.hpp>

Lust::SandCoffin2D::SandCoffin2D() :
	Layer("SandCoffin2D")
{

}

Lust::SandCoffin2D::~SandCoffin2D()
{

}

void Lust::SandCoffin2D::OnAttach()
{
	auto window = Application::GetInstance()->GetWindow();
	m_CameraController.reset(new OrthographicCameraController(window->GetWidth(), window->GetHeight(), true));
}

void Lust::SandCoffin2D::OnDetach()
{
	m_CameraController.reset();
}

void Lust::SandCoffin2D::OnUpdate(Timestep ts)
{
	m_CameraController->OnUpdate(ts);

	Renderer2D::BeginScene(m_CameraController->GetCamera());
	Renderer2D::DrawQuad(Eigen::Vector3f(0.0f, 0.0f, 0.0f), Eigen::Vector2f(150.0f, 150.0f), m_Renderer2DColor);
	Renderer2D::DrawQuad(Eigen::Vector3f(450.0f, 200.0f, 0.0f), Eigen::Vector2f(100.0f, 130.0f), Eigen::Vector4f(.8f, .2f, .3f, 1.0f));
	Renderer2D::EndScene();
}

void Lust::SandCoffin2D::OnImGuiRender()
{
	ImGui::Begin("Color Editor");
	ImGui::ColorEdit4("Square Color", &m_Renderer2DColor(0));
	ImGui::End();
}

void Lust::SandCoffin2D::OnEvent(Event& event)
{
	m_CameraController->OnEvent(event);
	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<WindowResizedEvent>(std::bind(&SandCoffin2D::OnWindowResize, this, std::placeholders::_1), true);
}

bool Lust::SandCoffin2D::OnWindowResize(WindowResizedEvent& e)
{
	return false;
}
