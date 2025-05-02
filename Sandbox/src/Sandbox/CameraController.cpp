#include "CameraController.hpp"
#include "Console.hpp"

void Lust::CameraController::OnCreate()
{
	Console::Debug("Created");
}

void Lust::CameraController::OnDestroy()
{
	Console::Debug("Destroyed");
}

void Lust::CameraController::OnUpdate(Timestep ts)
{
	Console::Debug("Updated");
}
