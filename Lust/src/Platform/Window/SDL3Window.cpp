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
	result = SDL_Init(SDL_INIT_EVENTS | SDL_INIT_JOYSTICK);
	assert(result);

	Uint32 window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;
	m_Window = SDL_CreateWindow(m_Title.c_str(), m_Width, m_Height, window_flags);
	assert(m_Window != nullptr);

	StartJoysticks();

	SDL_ShowWindow(m_Window);
}

Lust::SDL3Window::~SDL3Window()
{
	for (auto& joystick : m_Joysticks)
	{
		SDL_CloseJoystick(joystick.second.Joystick);
	}
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

void Lust::SDL3Window::SetEventCallback(const EventCallbackFn& callback)
{
	m_ExecuteCallback = callback;
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
		switch (eventData.type)
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
				WindowResizeEvent e(eventData.window.data1, eventData.window.data2);
				m_Width = eventData.window.data1;
				m_Height = eventData.window.data2;
				m_ExecuteCallback(e);
				break;
			}
			case SDL_EVENT_JOYSTICK_AXIS_MOTION:
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
				const SDL_JoystickID which = eventData.jaxis.which;
				Console::CoreDebug("Joystick Axis #{} axis {} -> {}", (unsigned int)which, (int)eventData.jaxis.axis, (int)eventData.jaxis.value);
				break;
			}
			
			case SDL_EVENT_JOYSTICK_ADDED:
			{
				const SDL_JoystickID which = eventData.jdevice.which;
				m_Joysticks[which].Joystick = SDL_OpenJoystick(which);
				m_Joysticks[which].JoystickVendor = SDL_GetJoystickVendor(m_Joysticks[which].Joystick);
				m_Joysticks[which].JoystickProduct = SDL_GetJoystickProduct(m_Joysticks[which].Joystick);
				break;
			}
			case SDL_EVENT_JOYSTICK_REMOVED:
			{
				const SDL_JoystickID which = eventData.jdevice.which;
				SDL_CloseJoystick(m_Joysticks[which].Joystick);  /* the joystick was unplugged. */
				auto it = m_Joysticks.find(which);
				if(it != m_Joysticks.end())
					m_Joysticks.erase(it);
				break;
			}
			case SDL_EVENT_JOYSTICK_BUTTON_UP:
			{
				const SDL_JoystickID which = eventData.jbutton.which;
				JoystickKeyReleasedEvent e(which, eventData.jbutton.button, eventData.jbutton.down ? 1 : 0);
				m_ExecuteCallback(e);
				break;
			}
			case SDL_EVENT_JOYSTICK_BUTTON_DOWN:
			{
				const SDL_JoystickID which = eventData.jbutton.which;
				JoystickKeyPressedEvent e(which, eventData.jbutton.button, eventData.jbutton.down? 1 : 0);
				m_ExecuteCallback(e);
				break;
				//Console::CoreDebug("Joystick #{} button {} -> {}", (unsigned int)which, (int)eventData.jbutton.button, eventData.jbutton.down ? "PRESSED" : "RELEASED");
			}
			case SDL_EVENT_JOYSTICK_HAT_MOTION:
			{
				const SDL_JoystickID which = eventData.jaxis.which;
				Console::CoreDebug("Joystick Hat #{} hat {} -> {}", (unsigned int)which, (int)eventData.jhat.hat, (int)eventData.jhat.value);
				break;
			}
		}	
	}
}

void Lust::SDL3Window::StartJoysticks()
{
	int joystickNumber;
	auto joystickIDs = SDL_GetJoysticks(&joystickNumber);

	for (size_t i = 0; i < joystickNumber; i++)
	{
		m_Joysticks[joystickIDs[i]].Joystick = SDL_OpenJoystick(joystickIDs[i]);
		assert(m_Joysticks[joystickIDs[i]].Joystick != nullptr);
		//usb_ids.h l.42
		m_Joysticks[joystickIDs[i]].JoystickVendor = SDL_GetJoystickVendor(m_Joysticks[joystickIDs[i]].Joystick);
		m_Joysticks[joystickIDs[i]].JoystickProduct = SDL_GetJoystickProduct(m_Joysticks[joystickIDs[i]].Joystick);
	}

	SDL_free(joystickIDs);
}
