#include "Controller.hpp"
#include "GameLayer.hpp"
#include <CompilerExceptions.hpp>
#include <Console.hpp>
#include <Compiler.hpp>

Controller::Controller()
{
	try
	{
	}
	catch (Lust::CompilerException e)
	{
	}

	PushLayer(new GameLayer());
}

Controller::~Controller()
{
}