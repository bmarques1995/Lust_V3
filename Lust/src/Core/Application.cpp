#include "Application.hpp"
#include <Console.hpp>
#include <iostream>

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
	Console::CoreDebug("Hello Lust");
}