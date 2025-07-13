#include "PlayerController.hpp"
#include "Input.hpp"
#include "MathComponents.hpp"
#include "TransformComponent.hpp"

void Lust::PlayerController::SetPlayer(Entity* entity)
{
	m_Player = entity;
}

void Lust::PlayerController::OnCreate()
{
	m_Player = nullptr;
}

void Lust::PlayerController::OnDestroy()
{
	m_Player = nullptr;
}

void Lust::PlayerController::OnUpdate(Timestep ts)
{
	static const float maxAxis = 32768.0f;
	if (m_Player)
	{
		float leftStickX = (float)Input::GetGamepadAxis(Gamepad::LUST_GAMEPAD_AXIS_LEFTX);
		float leftStickY = (float)Input::GetGamepadAxis(Gamepad::LUST_GAMEPAD_AXIS_LEFTY);
		vec2 leftStick(leftStickX, leftStickY);

		auto transform = m_Player->GetComponent<TransformComponent>();
		auto pos = transform->GetPosition();
		auto moddedPos = pos;

		if ((leftStick.norm() > maxAxis * Input::GetGamepadLowerDeadZone()) && (leftStick.norm() < maxAxis * Input::GetGamepadUpperDeadZone()))
		{
			leftStick.normalize();
			moddedPos.block<2, 1>(0, 0) += leftStick * m_CameraTranslationSpeed * ts;
		}

		if (Input::IsKeyPressed(Key::LUST_KEYCODE_A))
			moddedPos(0) -= m_CameraTranslationSpeed * ts;
		if (Input::IsKeyPressed(Key::LUST_KEYCODE_D))
			moddedPos(0) += m_CameraTranslationSpeed * ts;
		if (Input::IsKeyPressed(Key::LUST_KEYCODE_S))
			moddedPos(1) -= m_CameraTranslationSpeed * ts;
		if (Input::IsKeyPressed(Key::LUST_KEYCODE_W))
			moddedPos(1) += m_CameraTranslationSpeed * ts;

		if(moddedPos != pos)
			transform->SetPosition(moddedPos);
	}
}

void Lust::PlayerController::OnEvent(Event* e)
{

}

void Lust::PlayerController::OnCommand()
{

}