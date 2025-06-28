#include "SandCoffin.hpp"
#include "SandCoffin2D.hpp"

#include "Instrumentator.hpp"
#include <QMessageBox>
#include <functional>

Lust::SandCoffin::SandCoffin(int argc, char** argv)
{
	PushLayer(new SandCoffin2D());
}

Lust::SandCoffin::~SandCoffin()
{
}
