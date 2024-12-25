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
	//dispatcher.Dispatch<WindowResizeEvent>(HZ_BIND_EVENT_FN(Application::OnWindowResize));
}

bool Lust::Application::OnWindowClose(WindowCloseEvent& e)
{
	m_Running = false;
	return true;
}
