#include "OrthographicCameraController.hpp"
#include "Input.hpp"

Lust::OrthographicCameraController::OrthographicCameraController(float width, float height, bool rotation) :
	m_Width(width),
	m_Height(height),
	m_Camera(-width * m_ZoomLevel * .5f, width * m_ZoomLevel * .5f, -m_ZoomLevel * height * .5f, m_ZoomLevel * height * .5f),
	m_Rotation(rotation)
{
}

void Lust::OrthographicCameraController::OnUpdate(Timestep ts)
{
	static float maxAxis = 32768.0f;

	if (m_Rotation)
	{	
		float rightStickX = (float)Input::GetGamepadAxis(Gamepad::LUST_GAMEPAD_AXIS_RIGHTX);
		float rightStickY = (float)Input::GetGamepadAxis(Gamepad::LUST_GAMEPAD_AXIS_RIGHTY);
		vec2 rightStick(rightStickX, rightStickY);
		if ((rightStick.norm() > maxAxis * Input::GetGamepadLowerDeadZone()) && (rightStick.norm() < maxAxis * Input::GetGamepadUpperDeadZone()))
		{
			rightStick.normalize();
			m_CameraRotation += rightStick(0) * m_CameraRotationSpeed * ts;
		}

		if (Input::IsKeyPressed(Key::LUST_KEYCODE_J))
			m_CameraRotation += m_CameraRotationSpeed * ts;
		if (Input::IsKeyPressed(Key::LUST_KEYCODE_L))
			m_CameraRotation -= m_CameraRotationSpeed * ts;
	}
	
	/*float leftStickX = (float)Input::GetGamepadAxis(Gamepad::LUST_GAMEPAD_AXIS_LEFTX);
	float leftStickY = (float)Input::GetGamepadAxis(Gamepad::LUST_GAMEPAD_AXIS_LEFTY);
	vec2 leftStick(leftStickX, leftStickY);
	if ((leftStick.norm() > maxAxis * Input::GetGamepadLowerDeadZone()) && (leftStick.norm() < maxAxis * Input::GetGamepadUpperDeadZone()))
	{
		leftStick.normalize();
		m_CameraPosition.block<2, 1>(0, 0) += leftStick * m_CameraTranslationSpeed * ts;
	}

	if (Input::IsKeyPressed(Key::LUST_KEYCODE_A))
		m_CameraPosition(0) -= m_CameraTranslationSpeed * ts;
	if (Input::IsKeyPressed(Key::LUST_KEYCODE_D))
		m_CameraPosition(0) += m_CameraTranslationSpeed * ts;
	if (Input::IsKeyPressed(Key::LUST_KEYCODE_S))
		m_CameraPosition(1) -= m_CameraTranslationSpeed * ts;
	if (Input::IsKeyPressed(Key::LUST_KEYCODE_W))
		m_CameraPosition(1) += m_CameraTranslationSpeed * ts;*/

	if ((m_CameraRotation != m_CameraPreviousRotation) || (m_CameraPosition != m_CameraPreviousPosition))
	{
		m_Camera.SetPosition(m_CameraPosition);
		m_CameraPreviousPosition = m_CameraPosition;
		if (m_Rotation)
		{
			m_Camera.SetRotation(m_CameraRotation);
			m_CameraPreviousRotation = m_CameraRotation;
		}
	}
}

void Lust::OrthographicCameraController::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<MouseScrolledEvent>(std::bind(&OrthographicCameraController::OnMouseScroll, this, std::placeholders::_1), false);
	dispatcher.Dispatch<WindowResizedEvent>(std::bind(&OrthographicCameraController::OnWindowResize, this, std::placeholders::_1), false);
}

const Lust::OrthographicCamera& Lust::OrthographicCameraController::GetCamera() const
{
	return m_Camera;
}

void Lust::OrthographicCameraController::ResetCamera()
{
	m_ZoomLevel = 1.0f;
	m_CameraPosition = { 0.0f, 0.0f, 0.0f };
	m_CameraRotation = 0.0f;
	m_Camera.SetProjection(-m_Width * m_ZoomLevel * .5f, m_Width * m_ZoomLevel * .5f, -m_ZoomLevel * m_Height * .5f, m_ZoomLevel * m_Height * .5f);
	m_Camera.SetPositionAndRotation(m_CameraPosition, m_CameraRotation);
}

void Lust::OrthographicCameraController::SetPositionAndRotation(vec3 position, float rotation)
{
	m_CameraPosition = position;
	m_CameraRotation = rotation;
}

bool Lust::OrthographicCameraController::OnMouseScroll(MouseScrolledEvent& e)
{
	m_ZoomLevel -= e.GetYOffset() * .1f;
	m_Camera.SetProjection(-m_Width * m_ZoomLevel * .5f, m_Width * m_ZoomLevel * .5f, -m_ZoomLevel * m_Height * .5f, m_ZoomLevel * m_Height * .5f);
	return false;
}

bool Lust::OrthographicCameraController::OnWindowResize(WindowResizedEvent& e)
{
	m_Width = (float)e.GetWidth();
	m_Height = (float)e.GetHeight();
	m_Camera.SetProjection(-m_Width * m_ZoomLevel * .5f, m_Width * m_ZoomLevel * .5f, -m_ZoomLevel * m_Height * .5f, m_ZoomLevel * m_Height * .5f);
	return false;
}
