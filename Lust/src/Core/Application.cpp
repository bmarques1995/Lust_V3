#include "Application.hpp"
#include <Console.hpp>
#include <iostream>
#include <thread>
#include <chrono>
#include "Input.hpp"

Lust::Application* Lust::Application::s_AppSingleton = nullptr;
bool Lust::Application::s_SingletonEnabled = false;

Lust::Application::Application()
{
	EnableSingleton(this);
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
	while (m_Running)
	{
		m_Window->OnUpdate();
		if(Input::IsKeyPressed(Key::LUST_KEYCODE_ESCAPE))
			Console::CoreDebug("Escape key pressed");
		if(Input::IsMouseButtonPressed(Mouse::LUST_BUTTON_LEFT))
			Console::CoreDebug("({}, {})", Input::GetMousePosition().first, Input::GetMousePosition().second);
		if(Input::IsGamepadKeyPressed(Gamepad::LUST_GAMEPAD_BUTTON_SOUTH))
			Console::CoreDebug("A/Cross button pressed");
		int16_t leftStickX = Input::GetGamepadAxis(Gamepad::LUST_GAMEPAD_AXIS_LEFTX);
		int16_t leftStickY = Input::GetGamepadAxis(Gamepad::LUST_GAMEPAD_AXIS_LEFTY);
		if((leftStickX > 980) && (leftStickY > 980))
			Console::CoreDebug("Left joystick moved ({},{})", leftStickX, leftStickY);
	}
}

void Lust::Application::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowCloseEvent>(std::bind(&Application::OnWindowClose, this, std::placeholders::_1));
	dispatcher.Dispatch<WindowResizeEvent>(std::bind(&Application::OnWindowResize, this, std::placeholders::_1));
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

void Lust::Application::EnableSingleton(Application* ptr)
{
	if (!s_SingletonEnabled)
	{
		s_SingletonEnabled = true;
		s_AppSingleton = ptr;
	}
}

Lust::Application* Lust::Application::GetInstance()
{
	return s_AppSingleton;
}
