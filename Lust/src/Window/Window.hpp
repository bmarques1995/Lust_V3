#pragma once

#include "LustDLLMacro.hpp"
#include "Event.hpp"

#include <sstream>
#include <any>

namespace Lust
{

	struct LUST_API WindowProps
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;

		WindowProps(const std::string& title = "Lust Engine",
			uint32_t width = 960,
			uint32_t height = 540)
			: Title(title), Width(width), Height(height)
		{
		}
	};

	// Interface representing a desktop system based Window
	class LUST_API Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() = default;

		virtual void OnUpdate() = 0;

		virtual const std::string& GetTitle() const = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual bool ShouldClose() const = 0;

		virtual const bool* TrackWindowClosing() const = 0;

		virtual void DisplayWindow() = 0;

		// Window attributes
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;

		virtual void SetFullScreen(bool fullScreen) = 0;

		virtual bool IsFullscreen() const = 0;

		virtual bool IsCursorDisplayed() const = 0;
		virtual void DisplayCursor(bool display) = 0;

		virtual std::any GetNativePointer() const = 0;
		virtual std::any GetInstance() const = 0;
		virtual std::any GetWindow() const = 0;

		virtual std::any GetGamepad(uint32_t player = 1) const = 0;

		virtual void ResetTitle(std::string newTitle) = 0;
		virtual bool IsMinimized() const = 0;

		static Window* Instantiate(const WindowProps& props = WindowProps());
	};

}