#include "ExampleLayer.hpp"

Lust::ExampleLayer::ExampleLayer() :
	Layer("Example")
{
}

void Lust::ExampleLayer::OnUpdate(Timestep ts)
{
	if (Input::IsKeyPressed(Key::LUST_KEYCODE_ESCAPE))
		Console::CoreDebug("Escape key pressed");
	if (Input::IsMouseButtonPressed(Mouse::LUST_BUTTON_LEFT))
		Console::CoreDebug("({}, {})", Input::GetMousePosition().first, Input::GetMousePosition().second);
	if (Input::IsGamepadKeyPressed(Gamepad::LUST_GAMEPAD_BUTTON_SOUTH))
		Console::CoreDebug("A/Cross button pressed");
	int16_t leftStickX = Input::GetGamepadAxis(Gamepad::LUST_GAMEPAD_AXIS_LEFTX);
	int16_t leftStickY = Input::GetGamepadAxis(Gamepad::LUST_GAMEPAD_AXIS_LEFTY);
	if ((leftStickX > 980) && (leftStickY > 980))
		Console::CoreDebug("Left joystick moved ({},{})", leftStickX, leftStickY);
}

void Lust::ExampleLayer::OnImGuiRender()
{
}

void Lust::ExampleLayer::OnEvent(Event& event)
{
}


