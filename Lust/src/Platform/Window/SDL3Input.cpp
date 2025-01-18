#include "SDL3Input.hpp"
#include "Application.hpp"
#include "pch.hpp"

const std::unordered_map<Lust::KeyCode, SDL_Scancode> Lust::SDL3Input::s_KeyMap =
{
	{Key::LUST_KEYCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN},

	{Key::LUST_KEYCODE_A, SDL_SCANCODE_A},
	{Key::LUST_KEYCODE_B, SDL_SCANCODE_B},
	{Key::LUST_KEYCODE_C, SDL_SCANCODE_C},
	{Key::LUST_KEYCODE_D, SDL_SCANCODE_D},
	{Key::LUST_KEYCODE_E, SDL_SCANCODE_E},
	{Key::LUST_KEYCODE_F, SDL_SCANCODE_F},
	{Key::LUST_KEYCODE_G, SDL_SCANCODE_G},
	{Key::LUST_KEYCODE_H, SDL_SCANCODE_H},
	{Key::LUST_KEYCODE_I, SDL_SCANCODE_I},
	{Key::LUST_KEYCODE_J, SDL_SCANCODE_J},
	{Key::LUST_KEYCODE_K, SDL_SCANCODE_K},
	{Key::LUST_KEYCODE_L, SDL_SCANCODE_L},
	{Key::LUST_KEYCODE_M, SDL_SCANCODE_M},
	{Key::LUST_KEYCODE_N, SDL_SCANCODE_N},
	{Key::LUST_KEYCODE_O, SDL_SCANCODE_O},
	{Key::LUST_KEYCODE_P, SDL_SCANCODE_P},
	{Key::LUST_KEYCODE_Q, SDL_SCANCODE_Q},
	{Key::LUST_KEYCODE_R, SDL_SCANCODE_R},
	{Key::LUST_KEYCODE_S, SDL_SCANCODE_S},
	{Key::LUST_KEYCODE_T, SDL_SCANCODE_T},
	{Key::LUST_KEYCODE_U, SDL_SCANCODE_U},
	{Key::LUST_KEYCODE_V, SDL_SCANCODE_V},
	{Key::LUST_KEYCODE_W, SDL_SCANCODE_W},
	{Key::LUST_KEYCODE_X, SDL_SCANCODE_X},
	{Key::LUST_KEYCODE_Y, SDL_SCANCODE_Y},
	{Key::LUST_KEYCODE_Z, SDL_SCANCODE_Z},

	{Key::LUST_KEYCODE_0, SDL_SCANCODE_0},
	{Key::LUST_KEYCODE_1, SDL_SCANCODE_1},
	{Key::LUST_KEYCODE_2, SDL_SCANCODE_2},
	{Key::LUST_KEYCODE_3, SDL_SCANCODE_3},
	{Key::LUST_KEYCODE_4, SDL_SCANCODE_4},
	{Key::LUST_KEYCODE_5, SDL_SCANCODE_5},
	{Key::LUST_KEYCODE_6, SDL_SCANCODE_6},
	{Key::LUST_KEYCODE_7, SDL_SCANCODE_7},
	{Key::LUST_KEYCODE_8, SDL_SCANCODE_8},
	{Key::LUST_KEYCODE_9, SDL_SCANCODE_9},

	{Key::LUST_KEYCODE_SPACE, SDL_SCANCODE_SPACE},
	{Key::LUST_KEYCODE_RETURN, SDL_SCANCODE_RETURN},
	{Key::LUST_KEYCODE_ESCAPE, SDL_SCANCODE_ESCAPE},
	{Key::LUST_KEYCODE_BACKSPACE, SDL_SCANCODE_BACKSPACE},
	{Key::LUST_KEYCODE_TAB, SDL_SCANCODE_TAB},

	{Key::LUST_KEYCODE_MINUS, SDL_SCANCODE_MINUS},
	{Key::LUST_KEYCODE_EQUALS, SDL_SCANCODE_EQUALS},
	{Key::LUST_KEYCODE_LEFTBRACKET, SDL_SCANCODE_LEFTBRACKET},
	{Key::LUST_KEYCODE_RIGHTBRACKET, SDL_SCANCODE_RIGHTBRACKET},
	{Key::LUST_KEYCODE_BACKSLASH, SDL_SCANCODE_BACKSLASH},
	{Key::LUST_KEYCODE_NONUSHASH, SDL_SCANCODE_NONUSHASH},
	{Key::LUST_KEYCODE_SEMICOLON, SDL_SCANCODE_SEMICOLON},
	{Key::LUST_KEYCODE_APOSTROPHE, SDL_SCANCODE_APOSTROPHE},
	{Key::LUST_KEYCODE_GRAVE, SDL_SCANCODE_GRAVE},
	{Key::LUST_KEYCODE_COMMA, SDL_SCANCODE_COMMA},
	{Key::LUST_KEYCODE_PERIOD, SDL_SCANCODE_PERIOD},
	{Key::LUST_KEYCODE_SLASH, SDL_SCANCODE_SLASH},

	{Key::LUST_KEYCODE_CAPSLOCK, SDL_SCANCODE_CAPSLOCK},
	
	{Key::LUST_KEYCODE_F1, SDL_SCANCODE_F1},
	{Key::LUST_KEYCODE_F2, SDL_SCANCODE_F2},
	{Key::LUST_KEYCODE_F3, SDL_SCANCODE_F3},
	{Key::LUST_KEYCODE_F4, SDL_SCANCODE_F4},
	{Key::LUST_KEYCODE_F5, SDL_SCANCODE_F5},
	{Key::LUST_KEYCODE_F6, SDL_SCANCODE_F6},
	{Key::LUST_KEYCODE_F7, SDL_SCANCODE_F7},
	{Key::LUST_KEYCODE_F8, SDL_SCANCODE_F8},
	{Key::LUST_KEYCODE_F9, SDL_SCANCODE_F9},
	{Key::LUST_KEYCODE_F10, SDL_SCANCODE_F10},
	{Key::LUST_KEYCODE_F11, SDL_SCANCODE_F11},
	{Key::LUST_KEYCODE_F12, SDL_SCANCODE_F12},

	{Key::LUST_KEYCODE_PRINTSCREEN, SDL_SCANCODE_PRINTSCREEN},
	{Key::LUST_KEYCODE_SCROLLLOCK, SDL_SCANCODE_SCROLLLOCK},
	{Key::LUST_KEYCODE_PAUSE, SDL_SCANCODE_PAUSE},
	{Key::LUST_KEYCODE_INSERT, SDL_SCANCODE_INSERT},
	{Key::LUST_KEYCODE_HOME, SDL_SCANCODE_HOME},
	{Key::LUST_KEYCODE_PAGEUP, SDL_SCANCODE_PAGEUP},
	{Key::LUST_KEYCODE_DELETE, SDL_SCANCODE_DELETE},
	{Key::LUST_KEYCODE_END, SDL_SCANCODE_END},
	{Key::LUST_KEYCODE_PAGEDOWN, SDL_SCANCODE_PAGEDOWN},
	{Key::LUST_KEYCODE_RIGHT, SDL_SCANCODE_RIGHT},
	{Key::LUST_KEYCODE_LEFT, SDL_SCANCODE_LEFT},
	{Key::LUST_KEYCODE_DOWN, SDL_SCANCODE_DOWN},
	{Key::LUST_KEYCODE_UP, SDL_SCANCODE_UP},

	{Key::LUST_KEYCODE_NUMLOCKCLEAR, SDL_SCANCODE_NUMLOCKCLEAR},
	{Key::LUST_KEYCODE_KP_DIVIDE, SDL_SCANCODE_KP_DIVIDE},
	{Key::LUST_KEYCODE_KP_MULTIPLY, SDL_SCANCODE_KP_MULTIPLY},
	{Key::LUST_KEYCODE_KP_MINUS, SDL_SCANCODE_KP_MINUS},
	{Key::LUST_KEYCODE_KP_PLUS, SDL_SCANCODE_KP_PLUS},
	{Key::LUST_KEYCODE_KP_ENTER, SDL_SCANCODE_KP_ENTER},
	{Key::LUST_KEYCODE_KP_1, SDL_SCANCODE_KP_1},
	{Key::LUST_KEYCODE_KP_2, SDL_SCANCODE_KP_2},
	{Key::LUST_KEYCODE_KP_3, SDL_SCANCODE_KP_3},
	{Key::LUST_KEYCODE_KP_4, SDL_SCANCODE_KP_4},
	{Key::LUST_KEYCODE_KP_5, SDL_SCANCODE_KP_5},
	{Key::LUST_KEYCODE_KP_6, SDL_SCANCODE_KP_6},
	{Key::LUST_KEYCODE_KP_7, SDL_SCANCODE_KP_7},
	{Key::LUST_KEYCODE_KP_8, SDL_SCANCODE_KP_8},
	{Key::LUST_KEYCODE_KP_9, SDL_SCANCODE_KP_9},
	{Key::LUST_KEYCODE_KP_0, SDL_SCANCODE_KP_0},
	{Key::LUST_KEYCODE_KP_PERIOD, SDL_SCANCODE_KP_PERIOD},

	{Key::LUST_KEYCODE_NONUSBACKSLASH, SDL_SCANCODE_NONUSBACKSLASH},
	{Key::LUST_KEYCODE_APPLICATION, SDL_SCANCODE_APPLICATION},
	{Key::LUST_KEYCODE_POWER, SDL_SCANCODE_POWER},
	{Key::LUST_KEYCODE_KP_EQUALS, SDL_SCANCODE_KP_EQUALS},
	{Key::LUST_KEYCODE_F13, SDL_SCANCODE_F13},
	{Key::LUST_KEYCODE_F14, SDL_SCANCODE_F14},
	{Key::LUST_KEYCODE_F15, SDL_SCANCODE_F15},
	{Key::LUST_KEYCODE_F16, SDL_SCANCODE_F16},
	{Key::LUST_KEYCODE_F17, SDL_SCANCODE_F17},
	{Key::LUST_KEYCODE_F18, SDL_SCANCODE_F18},
	{Key::LUST_KEYCODE_F19, SDL_SCANCODE_F19},
	{Key::LUST_KEYCODE_F20, SDL_SCANCODE_F20},
	{Key::LUST_KEYCODE_F21, SDL_SCANCODE_F21},
	{Key::LUST_KEYCODE_F22, SDL_SCANCODE_F22},
	{Key::LUST_KEYCODE_F23, SDL_SCANCODE_F23},
	{Key::LUST_KEYCODE_F24, SDL_SCANCODE_F24},
	{Key::LUST_KEYCODE_EXECUTE, SDL_SCANCODE_EXECUTE},
	{Key::LUST_KEYCODE_HELP, SDL_SCANCODE_HELP},
	{Key::LUST_KEYCODE_MENU, SDL_SCANCODE_MENU},
	{Key::LUST_KEYCODE_SELECT, SDL_SCANCODE_SELECT},
	{Key::LUST_KEYCODE_STOP, SDL_SCANCODE_STOP},
	{Key::LUST_KEYCODE_AGAIN, SDL_SCANCODE_AGAIN},
	{Key::LUST_KEYCODE_UNDO, SDL_SCANCODE_UNDO},
	{Key::LUST_KEYCODE_CUT, SDL_SCANCODE_CUT},
	{Key::LUST_KEYCODE_COPY, SDL_SCANCODE_COPY},
	{Key::LUST_KEYCODE_PASTE, SDL_SCANCODE_PASTE},
	{Key::LUST_KEYCODE_FIND, SDL_SCANCODE_FIND},
	{Key::LUST_KEYCODE_MUTE, SDL_SCANCODE_MUTE},
	{Key::LUST_KEYCODE_VOLUMEUP, SDL_SCANCODE_VOLUMEUP},
	{Key::LUST_KEYCODE_VOLUMEDOWN, SDL_SCANCODE_VOLUMEDOWN},
	{Key::LUST_KEYCODE_KP_COMMA, SDL_SCANCODE_KP_COMMA},
	{Key::LUST_KEYCODE_KP_EQUALSAS400, SDL_SCANCODE_KP_EQUALSAS400},
	{Key::LUST_KEYCODE_INTERNATIONAL1, SDL_SCANCODE_INTERNATIONAL1},
	{Key::LUST_KEYCODE_INTERNATIONAL2, SDL_SCANCODE_INTERNATIONAL2},
	{Key::LUST_KEYCODE_INTERNATIONAL3, SDL_SCANCODE_INTERNATIONAL3},
	{Key::LUST_KEYCODE_INTERNATIONAL4, SDL_SCANCODE_INTERNATIONAL4},
	{Key::LUST_KEYCODE_INTERNATIONAL5, SDL_SCANCODE_INTERNATIONAL5},
	{Key::LUST_KEYCODE_INTERNATIONAL6, SDL_SCANCODE_INTERNATIONAL6},
	{Key::LUST_KEYCODE_INTERNATIONAL7, SDL_SCANCODE_INTERNATIONAL7},
	{Key::LUST_KEYCODE_INTERNATIONAL8, SDL_SCANCODE_INTERNATIONAL8},
	{Key::LUST_KEYCODE_INTERNATIONAL9, SDL_SCANCODE_INTERNATIONAL9},
	{Key::LUST_KEYCODE_LANG1, SDL_SCANCODE_LANG1},
	{Key::LUST_KEYCODE_LANG2, SDL_SCANCODE_LANG2},
	{Key::LUST_KEYCODE_LANG3, SDL_SCANCODE_LANG3},
	{Key::LUST_KEYCODE_LANG4, SDL_SCANCODE_LANG4},
	{Key::LUST_KEYCODE_LANG5, SDL_SCANCODE_LANG5},
	{Key::LUST_KEYCODE_LANG6, SDL_SCANCODE_LANG6},
	{Key::LUST_KEYCODE_LANG7, SDL_SCANCODE_LANG7},
	{Key::LUST_KEYCODE_LANG8, SDL_SCANCODE_LANG8},
	{Key::LUST_KEYCODE_LANG9, SDL_SCANCODE_LANG9},
	{Key::LUST_KEYCODE_ALTERASE, SDL_SCANCODE_ALTERASE},
	{Key::LUST_KEYCODE_SYSREQ, SDL_SCANCODE_SYSREQ},
	{Key::LUST_KEYCODE_CANCEL, SDL_SCANCODE_CANCEL},
	{Key::LUST_KEYCODE_CLEAR, SDL_SCANCODE_CLEAR},
	{Key::LUST_KEYCODE_PRIOR, SDL_SCANCODE_PRIOR},
	{Key::LUST_KEYCODE_RETURN2, SDL_SCANCODE_RETURN2},
	{Key::LUST_KEYCODE_SEPARATOR, SDL_SCANCODE_SEPARATOR},
	{Key::LUST_KEYCODE_OUT, SDL_SCANCODE_OUT},
	{Key::LUST_KEYCODE_OPER, SDL_SCANCODE_OPER},
	{Key::LUST_KEYCODE_CLEARAGAIN, SDL_SCANCODE_CLEARAGAIN},
	{Key::LUST_KEYCODE_CRSEL, SDL_SCANCODE_CRSEL},
	{Key::LUST_KEYCODE_EXSEL, SDL_SCANCODE_EXSEL},

	{Key::LUST_KEYCODE_KP_00, SDL_SCANCODE_KP_00},
	{Key::LUST_KEYCODE_KP_000, SDL_SCANCODE_KP_000},
	{Key::LUST_KEYCODE_THOUSANDSSEPARATOR, SDL_SCANCODE_THOUSANDSSEPARATOR},
	{Key::LUST_KEYCODE_DECIMALSEPARATOR, SDL_SCANCODE_DECIMALSEPARATOR},
	{Key::LUST_KEYCODE_CURRENCYUNIT, SDL_SCANCODE_CURRENCYUNIT},
	{Key::LUST_KEYCODE_CURRENCYSUBUNIT, SDL_SCANCODE_CURRENCYSUBUNIT},
	{Key::LUST_KEYCODE_KP_LEFTPAREN, SDL_SCANCODE_KP_LEFTPAREN},
	{Key::LUST_KEYCODE_KP_RIGHTPAREN, SDL_SCANCODE_KP_RIGHTPAREN},
	{Key::LUST_KEYCODE_KP_TAB, SDL_SCANCODE_KP_TAB},
	{Key::LUST_KEYCODE_KP_BACKSPACE, SDL_SCANCODE_KP_BACKSPACE},
	{Key::LUST_KEYCODE_KP_A, SDL_SCANCODE_KP_A},
	{Key::LUST_KEYCODE_KP_B, SDL_SCANCODE_KP_B},
	{Key::LUST_KEYCODE_KP_C, SDL_SCANCODE_KP_C},
	{Key::LUST_KEYCODE_KP_D, SDL_SCANCODE_KP_D},
	{Key::LUST_KEYCODE_KP_E, SDL_SCANCODE_KP_E},
	{Key::LUST_KEYCODE_KP_F, SDL_SCANCODE_KP_F},
	{Key::LUST_KEYCODE_KP_XOR, SDL_SCANCODE_KP_XOR},
	{Key::LUST_KEYCODE_KP_POWER, SDL_SCANCODE_KP_POWER},
	{Key::LUST_KEYCODE_KP_PERCENT, SDL_SCANCODE_KP_PERCENT},
	{Key::LUST_KEYCODE_KP_LESS, SDL_SCANCODE_KP_LESS},
	{Key::LUST_KEYCODE_KP_GREATER, SDL_SCANCODE_KP_GREATER},
	{Key::LUST_KEYCODE_KP_AMPERSAND, SDL_SCANCODE_KP_AMPERSAND},
	{Key::LUST_KEYCODE_KP_DBLAMPERSAND, SDL_SCANCODE_KP_DBLAMPERSAND},
	{Key::LUST_KEYCODE_KP_VERTICALBAR, SDL_SCANCODE_KP_VERTICALBAR},
	{Key::LUST_KEYCODE_KP_DBLVERTICALBAR, SDL_SCANCODE_KP_DBLVERTICALBAR},
	{Key::LUST_KEYCODE_KP_COLON, SDL_SCANCODE_KP_COLON},
	{Key::LUST_KEYCODE_KP_HASH, SDL_SCANCODE_KP_HASH},
	{Key::LUST_KEYCODE_KP_SPACE, SDL_SCANCODE_KP_SPACE},
	{Key::LUST_KEYCODE_KP_AT, SDL_SCANCODE_KP_AT},
	{Key::LUST_KEYCODE_KP_EXCLAM, SDL_SCANCODE_KP_EXCLAM},
	{Key::LUST_KEYCODE_KP_MEMSTORE, SDL_SCANCODE_KP_MEMSTORE},
	{Key::LUST_KEYCODE_KP_MEMRECALL, SDL_SCANCODE_KP_MEMRECALL},
	{Key::LUST_KEYCODE_KP_MEMCLEAR, SDL_SCANCODE_KP_MEMCLEAR},
	{Key::LUST_KEYCODE_KP_MEMADD, SDL_SCANCODE_KP_MEMADD},
	{Key::LUST_KEYCODE_KP_MEMSUBTRACT, SDL_SCANCODE_KP_MEMSUBTRACT},
	{Key::LUST_KEYCODE_KP_MEMMULTIPLY, SDL_SCANCODE_KP_MEMMULTIPLY},
	{Key::LUST_KEYCODE_KP_MEMDIVIDE, SDL_SCANCODE_KP_MEMDIVIDE},
	{Key::LUST_KEYCODE_KP_PLUSMINUS, SDL_SCANCODE_KP_PLUSMINUS},
	{Key::LUST_KEYCODE_KP_CLEAR, SDL_SCANCODE_KP_CLEAR},
	{Key::LUST_KEYCODE_KP_CLEARENTRY, SDL_SCANCODE_KP_CLEARENTRY},
	{Key::LUST_KEYCODE_KP_BINARY, SDL_SCANCODE_KP_BINARY},
	{Key::LUST_KEYCODE_KP_OCTAL, SDL_SCANCODE_KP_OCTAL},
	{Key::LUST_KEYCODE_KP_DECIMAL, SDL_SCANCODE_KP_DECIMAL},
	{Key::LUST_KEYCODE_KP_HEXADECIMAL, SDL_SCANCODE_KP_HEXADECIMAL},

	{Key::LUST_KEYCODE_LCTRL, SDL_SCANCODE_LCTRL},
	{Key::LUST_KEYCODE_LSHIFT, SDL_SCANCODE_LSHIFT},
	{Key::LUST_KEYCODE_LALT, SDL_SCANCODE_LALT},
	{Key::LUST_KEYCODE_LGUI, SDL_SCANCODE_LGUI},
	{Key::LUST_KEYCODE_RCTRL, SDL_SCANCODE_RCTRL},
	{Key::LUST_KEYCODE_RSHIFT, SDL_SCANCODE_RSHIFT},
	{Key::LUST_KEYCODE_RALT, SDL_SCANCODE_RALT},
	{Key::LUST_KEYCODE_RGUI, SDL_SCANCODE_RGUI},
};

const std::unordered_map<uint16_t, uint32_t> Lust::SDL3Input::s_MouseButtonMap =
{
	{Mouse::LUST_BUTTON_LEFT, SDL_BUTTON_LEFT},
	{Mouse::LUST_BUTTON_MIDDLE, SDL_BUTTON_MIDDLE},
	{Mouse::LUST_BUTTON_RIGHT, SDL_BUTTON_RIGHT},
	{Mouse::LUST_BUTTON_X1, SDL_BUTTON_X1},
	{Mouse::LUST_BUTTON_X2, SDL_BUTTON_X2},
};

const std::unordered_map<Lust::GamepadKeyCode, SDL_GamepadButton> Lust::SDL3Input::s_GamepadKeyMap =
{
	{Gamepad::LUST_GAMEPAD_BUTTON_SOUTH, SDL_GAMEPAD_BUTTON_SOUTH},
	{Gamepad::LUST_GAMEPAD_BUTTON_NORTH, SDL_GAMEPAD_BUTTON_NORTH},
	{Gamepad::LUST_GAMEPAD_BUTTON_WEST, SDL_GAMEPAD_BUTTON_WEST},
	{Gamepad::LUST_GAMEPAD_BUTTON_EAST, SDL_GAMEPAD_BUTTON_EAST},
	{Gamepad::LUST_GAMEPAD_BUTTON_GUIDE, SDL_GAMEPAD_BUTTON_GUIDE},
	{Gamepad::LUST_GAMEPAD_BUTTON_START, SDL_GAMEPAD_BUTTON_START},
	{Gamepad::LUST_GAMEPAD_BUTTON_BACK, SDL_GAMEPAD_BUTTON_BACK},
	{Gamepad::LUST_GAMEPAD_BUTTON_LEFT_STICK, SDL_GAMEPAD_BUTTON_LEFT_STICK},
	{Gamepad::LUST_GAMEPAD_BUTTON_RIGHT_STICK, SDL_GAMEPAD_BUTTON_RIGHT_STICK},
	{Gamepad::LUST_GAMEPAD_BUTTON_LEFT_SHOULDER, SDL_GAMEPAD_BUTTON_LEFT_SHOULDER},
	{Gamepad::LUST_GAMEPAD_BUTTON_RIGHT_SHOULDER, SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER},
	{Gamepad::LUST_GAMEPAD_BUTTON_DPAD_UP, SDL_GAMEPAD_BUTTON_DPAD_UP},
	{Gamepad::LUST_GAMEPAD_BUTTON_DPAD_DOWN, SDL_GAMEPAD_BUTTON_DPAD_DOWN},
	{Gamepad::LUST_GAMEPAD_BUTTON_DPAD_LEFT, SDL_GAMEPAD_BUTTON_DPAD_LEFT},
	{Gamepad::LUST_GAMEPAD_BUTTON_DPAD_RIGHT, SDL_GAMEPAD_BUTTON_DPAD_RIGHT},
	{Gamepad::LUST_GAMEPAD_BUTTON_MISC1, SDL_GAMEPAD_BUTTON_MISC1},
	{Gamepad::LUST_GAMEPAD_BUTTON_RIGHT_PADDLE1, SDL_GAMEPAD_BUTTON_RIGHT_PADDLE1},
	{Gamepad::LUST_GAMEPAD_BUTTON_LEFT_PADDLE1, SDL_GAMEPAD_BUTTON_LEFT_PADDLE1},
	{Gamepad::LUST_GAMEPAD_BUTTON_RIGHT_PADDLE2, SDL_GAMEPAD_BUTTON_RIGHT_PADDLE2},
	{Gamepad::LUST_GAMEPAD_BUTTON_LEFT_PADDLE2, SDL_GAMEPAD_BUTTON_LEFT_PADDLE2},
	{Gamepad::LUST_GAMEPAD_BUTTON_TOUCHPAD, SDL_GAMEPAD_BUTTON_TOUCHPAD},
	{Gamepad::LUST_GAMEPAD_BUTTON_MISC1, SDL_GAMEPAD_BUTTON_MISC1},
	{Gamepad::LUST_GAMEPAD_BUTTON_MISC2, SDL_GAMEPAD_BUTTON_MISC2},
	{Gamepad::LUST_GAMEPAD_BUTTON_MISC3, SDL_GAMEPAD_BUTTON_MISC3},
	{Gamepad::LUST_GAMEPAD_BUTTON_MISC4, SDL_GAMEPAD_BUTTON_MISC4},
	{Gamepad::LUST_GAMEPAD_BUTTON_MISC5, SDL_GAMEPAD_BUTTON_MISC5},
	{Gamepad::LUST_GAMEPAD_BUTTON_MISC6, SDL_GAMEPAD_BUTTON_MISC6},
};

const std::unordered_map<Lust::GamepadAxisCode, SDL_GamepadAxis> Lust::SDL3Input::s_GamepadAxisMap =
{
	{Gamepad::LUST_GAMEPAD_AXIS_LEFTX, SDL_GAMEPAD_AXIS_LEFTX},
	{Gamepad::LUST_GAMEPAD_AXIS_LEFTY, SDL_GAMEPAD_AXIS_LEFTY},
	{Gamepad::LUST_GAMEPAD_AXIS_RIGHTX, SDL_GAMEPAD_AXIS_RIGHTX},
	{Gamepad::LUST_GAMEPAD_AXIS_RIGHTY, SDL_GAMEPAD_AXIS_RIGHTY},
	{Gamepad::LUST_GAMEPAD_AXIS_LEFT_TRIGGER, SDL_GAMEPAD_AXIS_LEFT_TRIGGER},
	{Gamepad::LUST_GAMEPAD_AXIS_RIGHT_TRIGGER, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER},
};

bool Lust::SDL3Input::IsKeyPressedImpl(KeyCode keycode)
{
	auto window = std::any_cast<SDL_Window*>(Application::GetInstance()->GetWindow()->GetWindow());
	auto state = SDL_GetKeyboardState(nullptr);
	SDL_Scancode nativeKey;
	auto it = s_KeyMap.find(keycode);
	if(it == s_KeyMap.end())
		return false;
	return state[it->second];
}

bool Lust::SDL3Input::IsMouseButtonPressedImpl(MouseCode button)
{
	auto window = std::any_cast<SDL_Window*>(Application::GetInstance()->GetWindow()->GetWindow());
	float mouseX, mouseY;
	Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);
	auto it = s_MouseButtonMap.find(button);
	if (it == s_MouseButtonMap.end())
		return false;
	return mouseState & SDL_BUTTON_MASK(it->second);
}

std::pair<float, float> Lust::SDL3Input::GetMousePositionImpl()
{
	auto window = std::any_cast<SDL_Window*>(Application::GetInstance()->GetWindow()->GetWindow());
	float mouseX, mouseY;
	Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);
	return std::make_pair(mouseX, mouseY);
}

bool Lust::SDL3Input::IsGamepadKeyPressedImpl(GamepadKeyCode keycode, uint32_t player)
{
	//GetController
	auto gamepad = std::any_cast<SDL_Gamepad*>(Application::GetInstance()->GetWindow()->GetGamepad(player));
	if(gamepad == nullptr)
		return false;
	//MapKey
	auto it = s_GamepadKeyMap.find(keycode);
	if (it == s_GamepadKeyMap.end())
		return false;
	return SDL_GetGamepadButton(gamepad, it->second);
}

Lust::GamepadAxisValue Lust::SDL3Input::GetGamepadAxisImpl(GamepadAxisCode axis, uint32_t player)
{
	//SDL_GetGamepadAxis();
	//MapAxis
	auto gamepad = std::any_cast<SDL_Gamepad*>(Application::GetInstance()->GetWindow()->GetGamepad(player));
	if (gamepad == nullptr)
		return false;
	//MapKey
	auto it = s_GamepadAxisMap.find(axis);
	if (it == s_GamepadAxisMap.end())
		return 0;
	return SDL_GetGamepadAxis(gamepad, it->second);
}
