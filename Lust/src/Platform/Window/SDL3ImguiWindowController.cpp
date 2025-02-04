#include "SDL3ImguiWindowController.hpp"
#include <imgui_impl_sdl3.h>

Lust::SDL3ImguiWindowController::SDL3ImguiWindowController(const std::shared_ptr<SDL3Window>* window):
	m_Window(window)
{
	ImGui_ImplSDL3_InitForOther(std::any_cast<SDL_Window*>((*m_Window)->GetWindow()));
}

Lust::SDL3ImguiWindowController::~SDL3ImguiWindowController()
{
	ImGui_ImplSDL3_Shutdown();
}

void Lust::SDL3ImguiWindowController::ReceiveInput()
{
	ImGui_ImplSDL3_NewFrame();
}

