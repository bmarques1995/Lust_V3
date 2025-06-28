#include "SDL3Window.hpp"
#include <cassert>

#ifdef LUST_USES_WINDOWS
#include <windows.h>
#endif
#include <ApplicationEvent.hpp>
#include <InputEvent.hpp>
#include "Console.hpp"
#include <imgui_impl_sdl3.h>
#include <mutex>

#include "SDL3Functions.hpp"

Lust::SDL3Window::SDL3Window(WindowProps props) : m_Gamepads()
{
	SDL3Functions::LoadSDL3Functions();

	m_Width = props.Width;
	m_Height = props.Height;
	m_Title = props.Title;
	m_ShouldClose = false;
	m_FullScreen = false;
	m_Minimized = false;
	m_CursorDisplayed = true;

	bool result;
	result = SDL_Init(SDL_INIT_EVENTS | SDL_INIT_GAMEPAD | SDL_INIT_JOYSTICK);
	assert(result);

	Uint32 window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;
	m_Window = SDL_CreateWindow(m_Title.c_str(), m_Width, m_Height, window_flags);
	assert(m_Window != nullptr);

	SDL_StartTextInput(m_Window);
}

Lust::SDL3Window::~SDL3Window()
{
	SDL3Functions::SDL_StopTextInputFn(m_Window);
	
	for (auto& gamepad : m_Gamepads)
	{
		SDL3Functions::SDL_CloseGamepadFn(gamepad.Gamepad);
	}

	SDL3Functions::SDL_DestroyWindowFn(m_Window);
	SDL3Functions::SDL_QuitFn();

	SDL3Functions::UnloadSDL3Functions();
}

uint32_t Lust::SDL3Window::GetWidth() const
{
	return m_Width;
}

uint32_t Lust::SDL3Window::GetHeight() const
{
	return m_Height;
}

bool Lust::SDL3Window::ShouldClose() const
{
	return m_ShouldClose;
}

const bool* Lust::SDL3Window::TrackWindowClosing() const
{
	return &m_ShouldClose;
}

void Lust::SDL3Window::EmitClose()
{
	m_ShouldClose = true;
}

void Lust::SDL3Window::DisplayWindow()
{
	SDL3Functions::SDL_ShowWindowFn(m_Window);
}

bool Lust::SDL3Window::IsCursorDisplayed() const
{
	return m_CursorDisplayed;
}

void Lust::SDL3Window::DisplayCursor(bool display)
{
	if (!m_FullScreen)
	{
		return;
	}
	if(display == m_CursorDisplayed)
		return;
	m_CursorDisplayed = display;
	if (m_CursorDisplayed)
		SDL3Functions::SDL_WarpMouseInWindowFn(m_Window, m_Width / 2.0f, m_Height / 2.0f);
	SDL3Functions::SDL_SetWindowRelativeMouseModeFn(m_Window, !m_CursorDisplayed);
}

std::any Lust::SDL3Window::GetNativePointer() const
{
#ifdef LUST_USES_WINDOWS
	return (HWND)SDL3Functions::SDL_GetPointerPropertyFn(SDL_GetWindowProperties(m_Window), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
#else
	return nullptr;
#endif
}

std::any Lust::SDL3Window::GetInstance() const
{
#ifdef LUST_USES_WINDOWS
	return (HINSTANCE)SDL3Functions::SDL_GetPointerPropertyFn(SDL_GetWindowProperties(m_Window), SDL_PROP_WINDOW_WIN32_INSTANCE_POINTER, NULL);
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
	uint32_t i = 0;
	
	if ((player - 1) < m_Gamepads.size())
		return m_Gamepads[(player - 1)].Gamepad;
	else
		return {};
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
	SDL3Functions::SDL_SetWindowFullscreenFn(m_Window, m_FullScreen);
	int width, height;
	SDL3Functions::SDL_GetWindowSizeFn(m_Window, &width, &height);
	m_Height = height;
	m_Width = width;
	if(!m_FullScreen)
		DisplayCursor(!m_FullScreen);
}

bool Lust::SDL3Window::IsFullscreen() const
{
	return m_FullScreen;
}

void Lust::SDL3Window::ResetTitle(std::string newTitle)
{
	m_Title = newTitle;
	SDL3Functions::SDL_SetWindowTitleFn(m_Window, m_Title.c_str());
}

bool Lust::SDL3Window::IsMinimized() const
{
	return m_Minimized;
}

void Lust::SDL3Window::OnUpdate()
{
	SDL_Event eventData;
	while (SDL3Functions::SDL_PollEventFn(&eventData))
	{
		ImGui_ImplSDL3_ProcessEvent(&eventData);
		ProcessEvents(&eventData);
	}
}

const std::string& Lust::SDL3Window::GetTitle() const
{
	return m_Title;
}

std::vector<Lust::GamepadWrapper>::iterator Lust::SDL3Window::GetGamepad(uint32_t nativeIndex)
{
	std::vector<Lust::GamepadWrapper>::iterator it = m_Gamepads.begin();
	for (auto it =  m_Gamepads.begin(); (it != m_Gamepads.end()); it++)
		if((it->NativeIndex == nativeIndex))
			return it;
	return it;
}

void Lust::SDL3Window::StartGamepads()
{
	int gamepadNumber;
	auto gamepadIDs = SDL3Functions::SDL_GetGamepadsFn(&gamepadNumber);

	for (size_t i = 0; i < gamepadNumber; i++)
	{
		GamepadWrapper gamepad;
		gamepad.Gamepad = SDL3Functions::SDL_OpenGamepadFn(gamepadIDs[i]);
		assert(gamepad.Gamepad != nullptr);
		//usb_ids.h l.42
		gamepad.GamepadVendor = SDL3Functions::SDL_GetGamepadVendorFn(gamepad.Gamepad);
		gamepad.GamepadProduct = SDL3Functions::SDL_GetGamepadProductFn(gamepad.Gamepad);
		gamepad.NativeIndex = i;
		m_Gamepads.push_back(gamepad);
	}

	Console::CoreDebug("Gamepads Avaliable: {}", gamepadNumber);

	SDL3Functions::SDL_freeFn(gamepadIDs);
}

void Lust::SDL3Window::ProcessEvents(SDL_Event* eventData)
{
	switch (eventData->type)
	{
	case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
	case SDL_EVENT_QUIT:
	{
		WindowClosedEvent e;
		m_ExecuteCallback(e);
		break;
	}
	case SDL_EVENT_WINDOW_RESIZED:
	{
		WindowResizedEvent e(eventData->window.data1, eventData->window.data2);
		m_Width = eventData->window.data1;
		m_Height = eventData->window.data2;
		m_ExecuteCallback(e);
		break;
	}
	case SDL_EVENT_WINDOW_MINIMIZED:
	{
		m_Minimized = true;
		break;
	}
	case SDL_EVENT_WINDOW_RESTORED:
	{
		m_Minimized = false;
		break;
	}
	case SDL_EVENT_WINDOW_MAXIMIZED:
	{
		break;
	}
	case SDL_EVENT_WINDOW_LEAVE_FULLSCREEN:
	{
		m_CursorDisplayed = true;
		SDL3Functions::SDL_WarpMouseInWindowFn(m_Window, m_Width / 2.0f, m_Height / 2.0f);
		SDL3Functions::SDL_SetWindowRelativeMouseModeFn(m_Window, !m_CursorDisplayed);
		break;
	}
	case SDL_EVENT_GAMEPAD_ADDED:
	{
		const SDL_JoystickID which = eventData->jdevice.which;
		GamepadWrapper gamepad;
		gamepad.Gamepad = SDL3Functions::SDL_OpenGamepadFn(which);
		assert(gamepad.Gamepad != nullptr);
		gamepad.GamepadVendor = SDL3Functions::SDL_GetGamepadVendorFn(gamepad.Gamepad);
		gamepad.GamepadProduct = SDL3Functions::SDL_GetGamepadProductFn(gamepad.Gamepad);
		gamepad.NativeIndex = which;
		m_Gamepads.push_back(gamepad);
		GamepadConnectedEvent e(which, gamepad.GamepadVendor, m_Gamepads.size());
		m_ExecuteCallback(e);
		Console::CoreDebug("Gamepad Added");
		break;
	}
	case SDL_EVENT_GAMEPAD_REMOVED:
	{
		const SDL_JoystickID which = eventData->jdevice.which;
		std::mutex m_Mutex;
		auto it = GetGamepad(which);
		uint32_t gamepadVendor = it->GamepadVendor;
		if (it != m_Gamepads.end())
		{
			SDL3Functions::SDL_CloseGamepadFn(it->Gamepad);
			SDL3Functions::SDL_CloseGamepadFn(it->Gamepad);
			m_Gamepads.erase(it);
		}
		Console::CoreDebug("Gamepad Removed");
		DisplayCursor(true);
		GamepadDisconnectedEvent e(which, gamepadVendor, m_Gamepads.size());
		m_ExecuteCallback(e);
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

void Lust::SDL3Window::RemoveGamepad(GamepadWrapper* gamepad, uint32_t nativeIndex)
{
}

void Lust::SDL3Window::AddGamepad(GamepadWrapper* gamepad, uint32_t nativeIndex)
{

}
