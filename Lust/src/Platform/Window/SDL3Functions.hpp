#pragma once

#include <SDL3/SDL.h>

typedef const bool* (*PFN_SDL_GetKeyboardState)(int* numkeys);
typedef unsigned int (*PFN_SDL_GetMouseState)(float* x, float* y);
typedef bool (*PFN_SDL_GetGamepadButton)(SDL_Gamepad* gamepad, SDL_GamepadButton button);
typedef short int (*PFN_SDL_GetGamepadAxis)(SDL_Gamepad* gamepad, SDL_GamepadAxis axis);
typedef bool (*PFN_SDL_PollEvent)(SDL_Event* event);
typedef SDL_JoystickID* (*PFN_SDL_GetGamepads)(int* numgamepads);
typedef SDL_Gamepad* (*PFN_SDL_OpenGamepad)(SDL_JoystickID device_index);
typedef void (*PFN_SDL_CloseGamepad)(SDL_Gamepad* gamepad);
typedef uint16_t (*PFN_SDL_GetGamepadVendor)(SDL_Gamepad* gamepad);
typedef uint16_t (*PFN_SDL_GetGamepadProduct)(SDL_Gamepad* gamepad);
typedef SDL_Window* (*PFN_SDL_CreateWindow)(const char* title, int w, int h, SDL_WindowFlags flags);
typedef bool (*PFN_SDL_Init)(SDL_InitFlags flags);
typedef void (*PFN_SDL_Quit)();
typedef bool (*PFN_SDL_StartTextInput)(SDL_Window* window);
typedef bool (*PFN_SDL_StopTextInput)(SDL_Window* window);
typedef void (*PFN_SDL_DestroyWindow)(SDL_Window* window);
typedef bool (*PFN_SDL_ShowWindow)(SDL_Window* window);
typedef bool (*PFN_SDL_SetWindowTitle)(SDL_Window* window, const char* title);
typedef void (*PFN_SDL_WarpMouseInWindow)(SDL_Window* window, float x, float y);
typedef bool (*PFN_SDL_SetWindowRelativeMouseMode)(SDL_Window* window, bool enabled);
typedef bool (*PFN_SDL_SetWindowFullscreen)(SDL_Window* window, bool fullscreen);
typedef bool (*PFN_SDL_GetWindowSize)(SDL_Window* window, int* w, int* h);
typedef void* (*PFN_SDL_GetPointerProperty)(SDL_PropertiesID properties, const char* name, void* defaultValue);
typedef SDL_PropertiesID(*PFN_SDL_GetWindowProperties)(SDL_Window* window);
typedef void (*PFN_SDL_free)(void* ptr);

namespace Lust
{
	class SDL3Functions
	{
	public:
		static void LoadSDL3Functions();
		static void UnloadSDL3Functions();

		static void LoadMockSDL3Functions();

		static bool IsLoaded();

		//HRESULT WINAPI CreateDXGIFactory2(UINT Flags, REFIID riid, _COM_Outptr_ void **ppFactory)

		static PFN_SDL_GetKeyboardState SDL_GetKeyboardStateFn;
		static PFN_SDL_GetMouseState SDL_GetMouseStateFn;
		static PFN_SDL_GetGamepadButton SDL_GetGamepadButtonFn;
		static PFN_SDL_GetGamepadAxis SDL_GetGamepadAxisFn;
		static PFN_SDL_PollEvent SDL_PollEventFn;
		static PFN_SDL_GetGamepads	SDL_GetGamepadsFn;
		static PFN_SDL_OpenGamepad	SDL_OpenGamepadFn;
		static PFN_SDL_CloseGamepad SDL_CloseGamepadFn;
		static PFN_SDL_GetGamepadVendor SDL_GetGamepadVendorFn;
		static PFN_SDL_GetGamepadProduct SDL_GetGamepadProductFn;
		static PFN_SDL_CreateWindow SDL_CreateWindowFn;
		static PFN_SDL_Init SDL_InitFn;
		static PFN_SDL_Quit SDL_QuitFn;
		static PFN_SDL_StartTextInput SDL_StartTextInputFn;
		static PFN_SDL_StopTextInput SDL_StopTextInputFn;
		static PFN_SDL_DestroyWindow SDL_DestroyWindowFn;
		static PFN_SDL_ShowWindow SDL_ShowWindowFn;
		static PFN_SDL_SetWindowTitle SDL_SetWindowTitleFn;
		static PFN_SDL_WarpMouseInWindow SDL_WarpMouseInWindowFn;
		static PFN_SDL_SetWindowRelativeMouseMode SDL_SetWindowRelativeMouseModeFn;
		static PFN_SDL_SetWindowFullscreen	SDL_SetWindowFullscreenFn;
		static PFN_SDL_GetWindowSize SDL_GetWindowSizeFn;
		static PFN_SDL_GetPointerProperty SDL_GetPointerPropertyFn;
		static PFN_SDL_GetWindowProperties	SDL_GetWindowPropertiesFn;
		static PFN_SDL_free SDL_freeFn;

		static bool s_IsLoaded;

	};
}
