#include "Window.hpp"
#include "SDL3Window.hpp"

Lust::Window* Lust::Window::Instantiate(const WindowProps& props)
{
	return new SDL3Window(props);
}
