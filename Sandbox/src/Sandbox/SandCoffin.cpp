#include "SandCoffin.hpp"
#include "SandCoffin2D.hpp"

Lust::SandCoffin::SandCoffin()
{
	PushLayer(new SandCoffin2D());
}

Lust::SandCoffin::~SandCoffin()
{
}
