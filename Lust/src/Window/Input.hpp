#pragma once

#include "LustDLLMacro.hpp"
#include <utility>
#include "KeyCodes.hpp"
#include "MouseCodes.hpp"
#include "GamepadCodes.hpp"

namespace Lust
{
	class LUST_API Input
	{
	public:
		static bool IsKeyPressed(KeyCode keycode) { return s_Instance->IsKeyPressedImpl(keycode); }
		static bool IsMouseButtonPressed(MouseCode button) { return s_Instance->IsMouseButtonPressedImpl(button); }
		static std::pair<float, float> GetMousePosition() { return s_Instance->GetMousePositionImpl(); }
		static bool IsGamepadKeyPressed(GamepadKeyCode button, uint32_t player = 1) { return s_Instance->IsGamepadKeyPressedImpl(button, player); }
		static int16_t GetGamepadAxis(GamepadAxisCode axis, uint32_t player = 1) { return s_Instance->GetGamepadAxisImpl(axis, player); }

		static Input* CreateInstance();

	protected:
		virtual bool IsKeyPressedImpl(KeyCode keycode) = 0;
		virtual bool IsMouseButtonPressedImpl(MouseCode button) = 0;
		virtual std::pair<float, float> GetMousePositionImpl() = 0;

		virtual bool IsGamepadKeyPressedImpl(GamepadKeyCode button, uint32_t player = 1) = 0;
		virtual int16_t GetGamepadAxisImpl(GamepadAxisCode axis, uint32_t player = 1) = 0;
	private:
		static Input* s_Instance;
	};
}