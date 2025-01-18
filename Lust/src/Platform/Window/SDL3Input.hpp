#pragma once

#include "Input.hpp"
#include <unordered_map>

namespace Lust
{
	class LUST_API SDL3Input : public Input
	{
	protected:
		bool IsKeyPressedImpl(KeyCode keycode) override;
		bool IsMouseButtonPressedImpl(MouseCode button) override;
		std::pair<float, float> GetMousePositionImpl() override;

		bool IsGamepadKeyPressedImpl(GamepadKeyCode keycode, uint32_t player) override;
		int16_t GetGamepadAxisImpl(GamepadAxisCode axis, uint32_t player) override;

	private:
		static const std::unordered_map<uint16_t, SDL_Scancode> s_KeyMap;
		static const std::unordered_map<uint16_t, uint32_t> s_MouseButtonMap;
		static const std::unordered_map<GamepadKeyCode, SDL_GamepadButton> s_GamepadKeyMap;
		static const std::unordered_map<GamepadAxisCode, SDL_GamepadAxis> s_GamepadAxisMap;
	};
}
