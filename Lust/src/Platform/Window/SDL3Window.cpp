#include "SDL3Window.hpp"
#include <cassert>

#ifdef LUST_CORE_WINDOWS
#include <windows.h>
#endif
#include <ApplicationEvent.hpp>
#include <InputEvent.hpp>
#include "Console.hpp"

Lust::SDL3Window::SDL3Window(WindowProps props)
{
	m_Width = props.Width;
	m_Height = props.Height;
	m_Title = props.Title;
	m_FullScreen = false;
	m_Minimized = false;

	bool result;
	result = SDL_Init(SDL_INIT_EVENTS | SDL_INIT_GAMEPAD | SDL_INIT_JOYSTICK);
	assert(result);

	Uint32 window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;
	m_Window = SDL_CreateWindow(m_Title.c_str(), m_Width, m_Height, window_flags);
	assert(m_Window != nullptr);

	StartGamepads();

	SDL_StartTextInput(m_Window);

	SDL_ShowWindow(m_Window);
}

Lust::SDL3Window::~SDL3Window()
{
	SDL_StopTextInput(m_Window);
	
	for (auto& gamepad : m_Gamepads)
	{
		SDL_CloseGamepad(gamepad.second.Gamepad);
	}

	SDL_DestroyWindow(m_Window);
}

uint32_t Lust::SDL3Window::GetWidth() const
{
	return m_Width;
}

uint32_t Lust::SDL3Window::GetHeight() const
{
	return m_Height;
}

std::any Lust::SDL3Window::GetNativePointer() const
{
#ifdef LUST_CORE_WINDOWS
	return (HWND)SDL_GetPointerProperty(SDL_GetWindowProperties(m_Window), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
#else
	return nullptr;
#endif
}

std::any Lust::SDL3Window::GetInstance() const
{
#ifdef LUST_CORE_WINDOWS
	return (HINSTANCE)SDL_GetPointerProperty(SDL_GetWindowProperties(m_Window), SDL_PROP_WINDOW_WIN32_INSTANCE_POINTER, NULL);
#else
	return nullptr;
#endif;
}

std::any Lust::SDL3Window::GetWindow() const
{
	return m_Window;
}

std::any Lust::SDL3Window::GetGamepad(uint32_t player) const
{
	auto it = m_Gamepads.find(player);
	if (it == m_Gamepads.end())
		return nullptr;
	return it->second.Gamepad;
}

void Lust::SDL3Window::SetEventCallback(const EventCallbackFn& callback)
{
	m_ExecuteCallback = callback;
}

void Lust::SDL3Window::SetFullScreen(bool fullScreen)
{
	if(m_FullScreen == fullScreen)
		return;
	m_FullScreen = fullScreen;
	SDL_SetWindowFullscreen(m_Window, m_FullScreen);
}

bool Lust::SDL3Window::IsFullscreen() const
{
	return m_FullScreen;
}

void Lust::SDL3Window::ResetTitle(std::string newTitle)
{
	m_Title = newTitle;
	SDL_SetWindowTitle(m_Window, m_Title.c_str());
}

bool Lust::SDL3Window::IsMinimized() const
{
	return m_Minimized;
}

void Lust::SDL3Window::OnUpdate()
{
	SDL_Event eventData;
	while (SDL_PollEvent(&eventData))
	{
		ProcessEvents(&eventData);
	}
}

void Lust::SDL3Window::StartGamepads()
{
	int gamepadNumber;
	auto gamepadIDs = SDL_GetGamepads(&gamepadNumber);

	for (size_t i = 0; i < gamepadNumber; i++)
	{
		m_Gamepads[gamepadIDs[i]].Gamepad = SDL_OpenGamepad(gamepadIDs[i]);
		assert(m_Gamepads[gamepadIDs[i]].Gamepad != nullptr);
		//usb_ids.h l.42
		m_Gamepads[gamepadIDs[i]].GamepadVendor = SDL_GetGamepadVendor(m_Gamepads[gamepadIDs[i]].Gamepad);
		m_Gamepads[gamepadIDs[i]].GamepadProduct = SDL_GetGamepadProduct(m_Gamepads[gamepadIDs[i]].Gamepad);
	}

	Console::CoreDebug("Gamepads Avaliable: {}", gamepadNumber);

	SDL_free(gamepadIDs);
}

void Lust::SDL3Window::ProcessEvents(SDL_Event* eventData)
{
	switch (eventData->type)
	{
	case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
	case SDL_EVENT_QUIT:
	{
		WindowCloseEvent e;
		m_ExecuteCallback(e);
		break;
	}
	case SDL_EVENT_WINDOW_RESIZED:
	{
		WindowResizeEvent e(eventData->window.data1, eventData->window.data2);
		m_Width = eventData->window.data1;
		m_Height = eventData->window.data2;
		m_ExecuteCallback(e);
		break;
	}
	

	case SDL_EVENT_GAMEPAD_ADDED:
	{
		const SDL_JoystickID which = eventData->jdevice.which;
		m_Gamepads[which].Gamepad = SDL_OpenGamepad(which);
		m_Gamepads[which].GamepadVendor = SDL_GetGamepadVendor(m_Gamepads[which].Gamepad);
		m_Gamepads[which].GamepadProduct = SDL_GetGamepadProduct(m_Gamepads[which].Gamepad);
		Console::CoreDebug("Gamepad Added");
		break;
	}
	case SDL_EVENT_GAMEPAD_REMOVED:
	{
		const SDL_JoystickID which = eventData->jdevice.which;
		SDL_CloseGamepad(m_Gamepads[which].Gamepad);  /* the joystick was unplugged. */
		auto it = m_Gamepads.find(which);
		if (it != m_Gamepads.end())
			m_Gamepads.erase(it);
		Console::CoreDebug("Gamepad Removed");
		break;
	}
	case SDL_EVENT_GAMEPAD_AXIS_MOTION:
	{
		/*
		* axis
		0 - LeftAnalogX
		1 - LeftAnalogY
		2 - RightAnalogX
		3 - RightAnalogY
		4 - LeftTrigger
		5 - RightTrigger
		*/
		const SDL_JoystickID which = eventData->gaxis.which;
		GamepadAxisMovedEvent e(which, eventData->gaxis.axis, eventData->gaxis.value);
		m_ExecuteCallback(e);
		break;
	}
	case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
	{
		const SDL_JoystickID which = eventData->gbutton.which;
		GamepadKeyPressedEvent e(which, eventData->gbutton.button, eventData->gbutton.down ? 1 : 0);
		m_ExecuteCallback(e);
		break;
	}
	case SDL_EVENT_GAMEPAD_BUTTON_UP:
	{
		const SDL_JoystickID which = eventData->gbutton.which;
		GamepadKeyReleasedEvent e(which, eventData->gbutton.button, eventData->gbutton.down ? 1 : 0);
		m_ExecuteCallback(e);
		break;
	}
	case SDL_EVENT_KEY_DOWN:
	{
		KeyPressedEvent e(eventData->key.key, eventData->key.repeat ? 1 : 0);
		m_ExecuteCallback(e);
		break;
	}
	case SDL_EVENT_KEY_UP:
	{
		KeyReleasedEvent e(eventData->key.key);
		m_ExecuteCallback(e);
		break;
	}
	case SDL_EVENT_TEXT_INPUT:
	{
		TextTypedEvent e(eventData->text.text);
		m_ExecuteCallback(e);
		break;
	}
	case SDL_EVENT_MOUSE_MOTION:
	{
		MouseMovedEvent e(eventData->motion.x, eventData->motion.y);
		m_ExecuteCallback(e);
		break;
	}
	case SDL_EVENT_MOUSE_BUTTON_DOWN:
	{
		MouseButtonPressedEvent e(eventData->button.button);
		m_ExecuteCallback(e);
		break;
	}
	case SDL_EVENT_MOUSE_BUTTON_UP:
	{
		MouseButtonReleasedEvent e(eventData->button.button);
		m_ExecuteCallback(e);
		break;
	}
	case SDL_EVENT_MOUSE_WHEEL:
	{
		MouseScrolledEvent e(eventData->wheel.x, eventData->wheel.y);
		m_ExecuteCallback(e);
		break;
	}
	default:
		break;
	}
}
