#include "SandCoffin.hpp"
#include "SandCoffin2D.hpp"
#include "ExampleLayer.hpp"
#include <Console.hpp>

Lust::SandCoffin::SandCoffin()
{
	PushLayer(new ExampleLayer());
	//PushLayer(new SandCoffin2D());
}

Lust::SandCoffin::~SandCoffin()
{
}
