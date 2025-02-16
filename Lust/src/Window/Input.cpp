#include "Input.hpp"
#include "SDL3Input.hpp"

Lust::Input* Lust::Input::s_Instance = Lust::Input::CreateInstance();
float Lust::Input::s_GamepadLowerDeadZonePercent = .05f;
float Lust::Input::s_GamepadUpperDeadZonePercent = 1.1f;

Lust::Input* Lust::Input::CreateInstance()
{
	return new SDL3Input();
}
