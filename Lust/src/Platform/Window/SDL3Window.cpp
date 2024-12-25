#include "SDL3Window.hpp"
#include <cassert>

#ifdef LUST_CORE_WINDOWS
#include <windows.h>
#endif
#include <ApplicationEvent.hpp>

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

	SDL_ShowWindow(m_Window);
}

Lust::SDL3Window::~SDL3Window()
{
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
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
			case SDL_EVENT_QUIT:
			{
				WindowCloseEvent e;
				m_ExecuteCallback(e);
				break;
			}
		}	
	}
}
