#pragma once

#include "Window.hpp"
#include <SDL3/SDL.h>
#include <queue>

namespace Lust
{
	struct GamepadWrapper
	{
		SDL_Gamepad* Gamepad;
		uint16_t GamepadVendor;
		uint16_t GamepadProduct;
		uint32_t NativeIndex;
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

		bool IsCursorDisplayed() const override;
		void DisplayCursor(bool display) override;

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

		std::vector<GamepadWrapper>::iterator GetGamepad(uint32_t nativeIndex);
	private:

		void StartGamepads();

		void ProcessEvents(SDL_Event* eventData);

		void RemoveGamepad(GamepadWrapper* gamepad, uint32_t nativeIndex);
		void AddGamepad(GamepadWrapper* gamepad, uint32_t nativeIndex);

		uint32_t m_Width;
		uint32_t m_Height;
		std::string m_Title;
		SDL_Window* m_Window;
		EventCallbackFn m_ExecuteCallback;
		bool m_ShouldClose;
		bool m_Minimized;
		bool m_FullScreen;
		bool m_CursorDisplayed;

		std::vector<GamepadWrapper> m_Gamepads;
		std::queue<GamepadWrapper> m_WaitingGamepads;
	};
}