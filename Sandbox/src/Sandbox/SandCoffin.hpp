#pragma once
#include <Application.hpp>
#include <Input.hpp>
#include <Console.hpp>

namespace Lust
{
	class ExampleLayer : public Layer
	{
	public:
		ExampleLayer()
			: Layer("Example")
		{
			
		}

		void OnUpdate(Timestep ts) override
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

		virtual void OnImGuiRender() override
		{

		}

		void OnEvent(Event& event) override
		{
			
		}
	private:

	};
	class LUST_SANDBOX_API SandCoffin : public Application
	{
	public:
		SandCoffin();
		~SandCoffin();
	};
}
