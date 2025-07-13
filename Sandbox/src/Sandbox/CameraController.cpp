#include "CameraController.hpp"
#include "Console.hpp"
#include "Application.hpp"
#include <CameraComponent.hpp>
#include <TransformComponent.hpp>

void Lust::CameraController::ResetCamera()
{
	m_CameraController->ResetCamera();
}

void Lust::CameraController::Follow(const Entity* entity)
{
	m_FollowEntity = entity;
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
	auto window = Application::GetInstance()->GetWindow();
	float width = static_cast<float>(window->GetWidth());
	float height = static_cast<float>(window->GetHeight());
	m_CameraController.reset(new OrthographicCameraController(window->GetWidth(), window->GetHeight(), true));
	auto proj = m_CameraController->GetCamera().GetProjectionMatrix();
	auto view = m_CameraController->GetCamera().GetViewMatrix();
	auto camera = AddComponent<CameraComponent>(Camera(proj, view));
	m_FollowEntity = nullptr;
}

void Lust::CameraController::OnDestroy()
{
	m_CameraController.reset();
}

void Lust::CameraController::OnUpdate(Timestep ts)
{
	m_CameraController->OnUpdate(ts);
	auto camera = GetComponent<CameraComponent>();
	if (m_FollowEntity)
	{
		auto transform = m_FollowEntity->GetConstComponent<TransformComponent>();
		m_CameraController->SetPositionAndRotation(transform->GetPosition(), 0.0f);
	}
	camera->CameraElement.ResetProjectionMatrix(m_CameraController->GetCamera().GetProjectionMatrix());
	camera->CameraElement.ResetViewMatrix(m_CameraController->GetCamera().GetViewMatrix());
}
