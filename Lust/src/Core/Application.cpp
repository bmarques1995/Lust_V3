#include "Application.hpp"
#include <Console.hpp>
#include <iostream>

Lust::Application::Application()
{
	Console::Init();
	m_Window.reset(Window::Instantiate());
	m_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
}

Lust::Application::~Application()
{
	m_Window.reset();
	Console::End();
}

void Lust::Application::Run()
{
	WindowResizeEvent e(1280, 720);
	Console::CoreWarn("Resize simulated event ({},{})", e.GetWidth(), e.GetHeight());
	Console::CoreDebug("Hello Lust");
	
	while (m_Running)
	{
		m_Window->OnUpdate();
	}
}

void Lust::Application::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowCloseEvent>(std::bind(&Application::OnWindowClose, this, std::placeholders::_1));
	dispatcher.Dispatch<WindowResizeEvent>(std::bind(&Application::OnWindowResize, this, std::placeholders::_1));
	dispatcher.Dispatch<JoystickKeyPressedEvent>(std::bind(&Application::OnJoystickKeyPress, this, std::placeholders::_1));
	dispatcher.Dispatch<JoystickKeyReleasedEvent>(std::bind(&Application::OnJoystickKeyRelease, this, std::placeholders::_1));
}

bool Lust::Application::OnWindowClose(WindowCloseEvent& e)
{
	m_Running = false;
	return true;
}

bool Lust::Application::OnWindowResize(WindowResizeEvent& e)
{
	Console::CoreLog("New window size: ({},{})", e.GetWidth(), e.GetHeight());
	return true;
}

bool Lust::Application::OnJoystickKeyPress(JoystickKeyPressedEvent& e)
{
	Console::CoreDebug("Joystick #{} button {} -> {}", (unsigned int)e.GetJoystickNumber(), (int)e.GetKeyCode(), "PRESSED");
	return true;
}

bool Lust::Application::OnJoystickKeyRelease(JoystickKeyReleasedEvent& e)
{
	Console::CoreDebug("Joystick #{} button {} -> {}", (unsigned int)e.GetJoystickNumber(), (int)e.GetKeyCode(), "RELEASED");
	return true;
}

