#include "Application.hpp"
#include <Console.hpp>
#include <iostream>
#include "ApplicationEvent.hpp"

Lust::Application::Application()
{
	Console::Init();
}

Lust::Application::~Application()
{
	Console::End();
}

void Lust::Application::Run()
{
	WindowResizeEvent e(1280, 720);
	Console::CoreWarn("Resize simulated event ({},{})", e.GetWidth(), e.GetHeight());
	Console::CoreDebug("Hello Lust");
}