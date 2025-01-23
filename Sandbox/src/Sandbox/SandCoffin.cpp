#include "SandCoffin.hpp"
#include "ExampleLayer.hpp"
#include <Console.hpp>

Lust::SandCoffin::SandCoffin()
{
	PushLayer(new ExampleLayer());
}

Lust::SandCoffin::~SandCoffin()
{
}
