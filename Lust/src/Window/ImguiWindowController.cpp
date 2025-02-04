#include "ImguiWindowController.hpp"
#include "SDL3ImguiWindowController.hpp"

Lust::ImguiWindowController* Lust::ImguiWindowController::Instantiate(const std::shared_ptr<Window>* window)
{
    return new SDL3ImguiWindowController((const std::shared_ptr<SDL3Window>*)window);
}
