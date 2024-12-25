#pragma once

#include "Event.hpp"

#include <sstream>
#include <any>

namespace Lust
{

	struct WindowProps
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
	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() = default;

		virtual void OnUpdate() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		// Window attributes
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;

		virtual std::any GetNativePointer() const = 0;
		virtual std::any GetInstance() const = 0;
		virtual std::any GetWindow() const = 0;

		virtual void ResetTitle(std::string newTitle) = 0;
		virtual bool IsMinimized() const = 0;

		static Window* Instantiate(const WindowProps& props = WindowProps());
	};

}