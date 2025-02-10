#include "ImguiWindowController.hpp"
#include "SDL3ImguiWindowController.hpp"

Lust::ImguiWindowController* Lust::ImguiWindowController::Instantiate(const Window* window)
{
    return new SDL3ImguiWindowController((const SDL3Window*)window);
}
