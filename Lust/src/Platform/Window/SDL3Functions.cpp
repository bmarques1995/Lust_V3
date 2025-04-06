#include "SDL3Functions.hpp"

PFN_SDL_GetKeyboardState Lust::SDL3Functions::SDL_GetKeyboardStateFn;
PFN_SDL_GetMouseState Lust::SDL3Functions::SDL_GetMouseStateFn;
PFN_SDL_GetGamepadButton Lust::SDL3Functions::SDL_GetGamepadButtonFn;
PFN_SDL_GetGamepadAxis Lust::SDL3Functions::SDL_GetGamepadAxisFn;
PFN_SDL_PollEvent Lust::SDL3Functions::SDL_PollEventFn;
PFN_SDL_GetGamepads	Lust::SDL3Functions::SDL_GetGamepadsFn;
PFN_SDL_OpenGamepad	Lust::SDL3Functions::SDL_OpenGamepadFn;
PFN_SDL_CloseGamepad Lust::SDL3Functions::SDL_CloseGamepadFn;
PFN_SDL_GetGamepadVendor Lust::SDL3Functions::SDL_GetGamepadVendorFn;
PFN_SDL_GetGamepadProduct Lust::SDL3Functions::SDL_GetGamepadProductFn;
PFN_SDL_CreateWindow Lust::SDL3Functions::SDL_CreateWindowFn;
PFN_SDL_Init Lust::SDL3Functions::SDL_InitFn;
PFN_SDL_Quit Lust::SDL3Functions::SDL_QuitFn;
PFN_SDL_StartTextInput Lust::SDL3Functions::SDL_StartTextInputFn;
PFN_SDL_StopTextInput Lust::SDL3Functions::SDL_StopTextInputFn;
PFN_SDL_DestroyWindow Lust::SDL3Functions::SDL_DestroyWindowFn;
PFN_SDL_ShowWindow Lust::SDL3Functions::SDL_ShowWindowFn;
PFN_SDL_SetWindowTitle Lust::SDL3Functions::SDL_SetWindowTitleFn;
PFN_SDL_WarpMouseInWindow Lust::SDL3Functions::SDL_WarpMouseInWindowFn;
PFN_SDL_SetWindowRelativeMouseMode Lust::SDL3Functions::SDL_SetWindowRelativeMouseModeFn;
PFN_SDL_SetWindowFullscreen	Lust::SDL3Functions::SDL_SetWindowFullscreenFn;
PFN_SDL_GetWindowSize Lust::SDL3Functions::SDL_GetWindowSizeFn;
PFN_SDL_GetPointerProperty Lust::SDL3Functions::SDL_GetPointerPropertyFn;
PFN_SDL_GetWindowProperties	Lust::SDL3Functions::SDL_GetWindowPropertiesFn;
PFN_SDL_free Lust::SDL3Functions::SDL_freeFn;
bool Lust::SDL3Functions::s_IsLoaded;

void Lust::SDL3Functions::LoadSDL3Functions()
{
	if (s_IsLoaded)
		return;

	SDL_GetKeyboardStateFn = SDL_GetKeyboardState;
	SDL_GetMouseStateFn = SDL_GetMouseState;
	SDL_GetGamepadButtonFn = SDL_GetGamepadButton;
	SDL_GetGamepadAxisFn = SDL_GetGamepadAxis;
	SDL_PollEventFn = SDL_PollEvent;
	SDL_GetGamepadsFn = SDL_GetGamepads;
	SDL_OpenGamepadFn = SDL_OpenGamepad;
	SDL_CloseGamepadFn = SDL_CloseGamepad;
	SDL_GetGamepadVendorFn = SDL_GetGamepadVendor;
	SDL_GetGamepadProductFn = SDL_GetGamepadProduct;
	SDL_CreateWindowFn = SDL_CreateWindow;
	SDL_InitFn = SDL_Init;
	SDL_QuitFn = SDL_Quit;
	SDL_StartTextInputFn = SDL_StartTextInput;
	SDL_StopTextInputFn = SDL_StopTextInput;
	SDL_DestroyWindowFn	= SDL_DestroyWindow;
	SDL_ShowWindowFn = SDL_ShowWindow;
	SDL_SetWindowTitleFn = SDL_SetWindowTitle;
	SDL_WarpMouseInWindowFn = SDL_WarpMouseInWindow;
	SDL_SetWindowRelativeMouseModeFn = SDL_SetWindowRelativeMouseMode;
	SDL_SetWindowFullscreenFn =	SDL_SetWindowFullscreen;
	SDL_GetWindowSizeFn = SDL_GetWindowSize;
	SDL_GetPointerPropertyFn = SDL_GetPointerProperty;
	SDL_GetWindowPropertiesFn = SDL_GetWindowProperties;
	SDL_freeFn = SDL_free;
}

void Lust::SDL3Functions::UnloadSDL3Functions()
{
	if (!s_IsLoaded)
		return;

	SDL_GetKeyboardStateFn = nullptr;
	SDL_GetMouseStateFn = nullptr;
	SDL_GetGamepadButtonFn = nullptr;
	SDL_GetGamepadAxisFn = nullptr;
	SDL_PollEventFn = nullptr;
	SDL_GetGamepadsFn = nullptr;
	SDL_OpenGamepadFn = nullptr;
	SDL_CloseGamepadFn = nullptr;
	SDL_GetGamepadVendorFn = nullptr;
	SDL_GetGamepadProductFn = nullptr;
	SDL_CreateWindowFn = nullptr;
	SDL_InitFn = nullptr;
	SDL_QuitFn = nullptr;
	SDL_StartTextInputFn = nullptr;
	SDL_StopTextInputFn = nullptr;
	SDL_DestroyWindowFn = nullptr;
	SDL_ShowWindowFn = nullptr;
	SDL_SetWindowTitleFn = nullptr;
	SDL_WarpMouseInWindowFn = nullptr;
	SDL_SetWindowRelativeMouseModeFn = nullptr;
	SDL_SetWindowFullscreenFn = nullptr;
	SDL_GetWindowSizeFn = nullptr;
	SDL_GetPointerPropertyFn = nullptr;
	SDL_GetWindowPropertiesFn = nullptr;
	SDL_freeFn = nullptr;

	s_IsLoaded = false;
}

void Lust::SDL3Functions::LoadMockSDL3Functions()
{
	if (s_IsLoaded)
		return;
}

bool Lust::SDL3Functions::IsLoaded()
{
	return false;
}
