#include "CameraController.hpp"
#include "Console.hpp"
#include "Application.hpp"

void Lust::CameraController::ResetCamera()
{
	m_CameraController->ResetCamera();
}

void Lust::CameraController::OnEvent(Event* e)
{
	m_CameraController->OnEvent(*e);
}

void Lust::CameraController::OnCommand()
{
	m_CameraController->ResetCamera();
}

void Lust::CameraController::OnCreate()
{
	Console::Debug("Created");
	auto window = Application::GetInstance()->GetWindow();
	float width = static_cast<float>(window->GetWidth());
	float height = static_cast<float>(window->GetHeight());
	m_CameraController.reset(new OrthographicCameraController(window->GetWidth(), window->GetHeight(), true));
	auto proj = m_CameraController->GetCamera().GetProjectionMatrix();
	auto view = m_CameraController->GetCamera().GetViewMatrix();
	auto camera = AddComponent<CameraComponent>(Camera(proj, view));
	
}

void Lust::CameraController::OnDestroy()
{
	m_CameraController.reset();
}

void Lust::CameraController::OnUpdate(Timestep ts)
{
	m_CameraController->OnUpdate(ts);
	auto camera = GetComponent<CameraComponent>();
	camera->CameraElement.ResetProjectionMatrix(m_CameraController->GetCamera().GetProjectionMatrix());
	camera->CameraElement.ResetViewMatrix(m_CameraController->GetCamera().GetViewMatrix());
}
