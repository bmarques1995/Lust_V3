#pragma once
#include <cstdint>
#include <SDL3/SDL.h>

namespace Lust
{
	using GamepadKeyCode = uint16_t;
	using GamepadKeyValue = int16_t;

	using GamepadAxisCode = uint16_t;
	using GamepadAxisValue = int16_t;

	namespace Gamepad
	{
		enum : GamepadKeyCode
		{
			LUST_GAMEPAD_BUTTON_SOUTH,           
			LUST_GAMEPAD_BUTTON_EAST,            
			LUST_GAMEPAD_BUTTON_WEST,            
			LUST_GAMEPAD_BUTTON_NORTH,           
			LUST_GAMEPAD_BUTTON_BACK,
			LUST_GAMEPAD_BUTTON_GUIDE,
			LUST_GAMEPAD_BUTTON_START,
			LUST_GAMEPAD_BUTTON_LEFT_STICK,
			LUST_GAMEPAD_BUTTON_RIGHT_STICK,
			LUST_GAMEPAD_BUTTON_LEFT_SHOULDER,
			LUST_GAMEPAD_BUTTON_RIGHT_SHOULDER,
			LUST_GAMEPAD_BUTTON_DPAD_UP,
			LUST_GAMEPAD_BUTTON_DPAD_DOWN,
			LUST_GAMEPAD_BUTTON_DPAD_LEFT,
			LUST_GAMEPAD_BUTTON_DPAD_RIGHT,
			LUST_GAMEPAD_BUTTON_MISC1,
			LUST_GAMEPAD_BUTTON_RIGHT_PADDLE1,
			LUST_GAMEPAD_BUTTON_LEFT_PADDLE1,
			LUST_GAMEPAD_BUTTON_RIGHT_PADDLE2,
			LUST_GAMEPAD_BUTTON_LEFT_PADDLE2,
			LUST_GAMEPAD_BUTTON_TOUCHPAD,
			LUST_GAMEPAD_BUTTON_MISC2,
			LUST_GAMEPAD_BUTTON_MISC3,
			LUST_GAMEPAD_BUTTON_MISC4,
			LUST_GAMEPAD_BUTTON_MISC5,
			LUST_GAMEPAD_BUTTON_MISC6,
			LUST_GAMEPAD_BUTTON_COUNT
		};

		enum : GamepadAxisCode
		{
			LUST_GAMEPAD_AXIS_LEFTX,
			LUST_GAMEPAD_AXIS_LEFTY,
			LUST_GAMEPAD_AXIS_RIGHTX,
			LUST_GAMEPAD_AXIS_RIGHTY,
			LUST_GAMEPAD_AXIS_LEFT_TRIGGER,
			LUST_GAMEPAD_AXIS_RIGHT_TRIGGER,
		};
	}
}