#include "Input.hpp"
#include "SDL3Input.hpp"

Lust::Input* Lust::Input::s_Instance = Lust::Input::CreateInstance();

Lust::Input* Lust::Input::CreateInstance()
{
	return new SDL3Input();
}
