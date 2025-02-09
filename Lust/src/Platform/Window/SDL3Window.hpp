#pragma once

#include "Window.hpp"
#include <SDL3/SDL.h>

namespace Lust
{
	struct GamepadWrapper
	{
		SDL_Gamepad* Gamepad;
		uint16_t GamepadVendor;
		uint16_t GamepadProduct;
	};
	
	class LUST_API SDL3Window : public Window
	{
	public:
		SDL3Window(WindowProps props);
		~SDL3Window();

		uint32_t GetWidth() const override;
		uint32_t GetHeight() const override;
		
		bool ShouldClose() const override;
		const bool* TrackWindowClosing() const override;

		std::any GetNativePointer() const override;
		std::any GetInstance() const override;
		std::any GetWindow() const override;

		std::any GetGamepad(uint32_t player) const override;

		void SetEventCallback(const EventCallbackFn& callback) override;

		void SetFullScreen(bool fullScreen) override;

		bool IsFullscreen() const override;

		void ResetTitle(std::string newTitle) override;
		bool IsMinimized() const override;
		
		void OnUpdate() override;

		const std::string& GetTitle() const override;

	private:

		void StartGamepads();

		void ProcessEvents(SDL_Event* eventData);

		uint32_t m_Width;
		uint32_t m_Height;
		std::string m_Title;
		SDL_Window* m_Window;
		EventCallbackFn m_ExecuteCallback;
		bool m_ShouldClose;
		bool m_Minimized;
		bool m_FullScreen;

		std::unordered_map<SDL_JoystickID, GamepadWrapper> m_Gamepads;
	};
}