#pragma once

#include "LustDLLMacro.hpp"
#include "Event.hpp"

#include <sstream>
#include <any>

namespace Lust
{
	/**
	* @brief %Window properties
	* @details Used to create a window
	* @param title The title of the window
	* @param width The width of the window
	* @param height The height of the window
	*/
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

	/**
	* @brief Interface representing a desktop system based %Window
	*/
	class LUST_API Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() = default;

		/**
		* @brief Update the window
		*/
		virtual void OnUpdate() = 0;

		/**
		* @brief Get the window title
		*/
		virtual const std::string& GetTitle() const = 0;

		/**
		* @brief Get the window width
		*/
		virtual uint32_t GetWidth() const = 0;
		/**
		* @brief Get the window height
		*/
		virtual uint32_t GetHeight() const = 0;

		/**
		* @brief Get the window handle
		*/
		virtual bool ShouldClose() const = 0;

		/**
		* @brief Get the window handle
		*/
		virtual const bool* TrackWindowClosing() const = 0;

		/**
		* @brief Display the window
		*/
		virtual void DisplayWindow() = 0;

		/**
		* @brief Set the window event callback
		*/
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;

		/**
		* @brief Set the window full screen
		*/
		virtual void SetFullScreen(bool fullScreen) = 0;

		/**
		* @brief Get if the window is in full screen mode
		*/
		virtual bool IsFullscreen() const = 0;

		/**
		* @brief Get if the cursor is displayed
		*/
		virtual bool IsCursorDisplayed() const = 0;
		/**
		* @brief Display the cursor
		*/
		virtual void DisplayCursor(bool display) = 0;

		/**
		* @brief Get the native window pointer
		*/
		virtual std::any GetNativePointer() const = 0;
		/**
		* @brief Get the window pointer
		*/
		virtual std::any GetInstance() const = 0;
		/**
		* @brief Get the window pointer
		*/
		virtual std::any GetWindow() const = 0;

		/**
		* @brief Get the gamepad pointer based on player
		*/
		virtual std::any GetGamepad(uint32_t player = 1) const = 0;

		/**
		* @brief Reset the window title
		* @param newTitle The new title
		*/
		virtual void ResetTitle(std::string newTitle) = 0;
		/**
		* @brief Get if the window is minimized
		*/
		virtual bool IsMinimized() const = 0;

		/**
		* @brief Instantiate a window
		* @param props The window properties
		*/
		static Window* Instantiate(const WindowProps& props = WindowProps());
	};

}