#pragma once

#include "Window.hpp"
#include <SDL3/SDL.h>

namespace Lust
{
	class LUST_API SDL3Window : public Window
	{
	public:
		SDL3Window(WindowProps props);
		~SDL3Window();

		uint32_t GetWidth() const override;
		uint32_t GetHeight() const override;
		
		std::any GetNativePointer() const override;
		std::any GetInstance() const override;
		std::any GetWindow() const override;

		void SetEventCallback(const EventCallbackFn& callback) override;

		void ResetTitle(std::string newTitle) override;
		bool IsMinimized() const override;
		
		void OnUpdate() override;

	private:

		uint32_t m_Width;
		uint32_t m_Height;
		std::string m_Title;
		SDL_Window* m_Window;
		EventCallbackFn m_ExecuteCallback;
		bool m_Minimized;
		bool m_FullScreen;
	};
}