#pragma once

#include "Event.hpp"
#include <sstream>

namespace Lust
{
	/**
	* @brief Class for window resize events
	* @details This event is dispatched when the window is resized. its event type is EventType::WindowResized and its category is LUST_EVENT_CATEGORY_APPLICATION
	*/
	class WindowResizedEvent : public Event
	{
	public:
		/**
		* @brief Constructor
		* @param width The new width of the window
		* @param height The new height of the window
		*/
		WindowResizedEvent(unsigned int width, unsigned int height)
			: m_Width(width), m_Height(height) {
		}

		/**
		* @brief Returns the new width of the window
		*/
		unsigned int GetWidth() const { return m_Width; }
		/**
		* @brief Returns the new height of the window
		*/
		unsigned int GetHeight() const { return m_Height; }

		/**
		* @brief Returns a string representation of the event
		*/
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowResized)
		EVENT_CLASS_CATEGORY(LUST_EVENT_CATEGORY_APPLICATION)
	private:
		unsigned int m_Width, m_Height;
	};

	/**
	* @brief Class for window close events
	* @details This event is dispatched when the window is closed. its event type is EventType::WindowClosed and its category is LUST_EVENT_CATEGORY_APPLICATION
	*/
	class WindowClosedEvent : public Event
	{
	public:
		/**
		* @brief Constructor
		*/
		WindowClosedEvent() = default;

		EVENT_CLASS_TYPE(WindowClosed)
		EVENT_CLASS_CATEGORY(LUST_EVENT_CATEGORY_APPLICATION)
	};

	/**
	* @brief Class for window minimize events
	* @details This event is dispatched when the window is minimized. its event type is EventType::WindowMinimized and its category is LUST_EVENT_CATEGORY_APPLICATION
	*/
	class AppTickEvent : public Event
	{
	public:
		/**
		* @brief Constructor
		*/
		AppTickEvent() = default;

		EVENT_CLASS_TYPE(AppTicked)
		EVENT_CLASS_CATEGORY(LUST_EVENT_CATEGORY_APPLICATION)
	};

	/**
	* @brief Class for window minimize events
	* @details This event is dispatched when the window is minimized. its event type is EventType::WindowMinimized and its category is LUST_EVENT_CATEGORY_APPLICATION
	*/
	class AppUpdateEvent : public Event
	{
	public:
		/**
		* @brief Constructor
		*/
		AppUpdateEvent() = default;

		EVENT_CLASS_TYPE(AppUpdated)
		EVENT_CLASS_CATEGORY(LUST_EVENT_CATEGORY_APPLICATION)
	};

	/**
	* @brief Class for window minimize events
	* @details This event is dispatched when the window is minimized. its event type is EventType::WindowMinimized and its category is LUST_EVENT_CATEGORY_APPLICATION
	*/
	class AppRenderEvent : public Event
	{
	public:
		/**
		* @brief Constructor
		*/
		AppRenderEvent() = default;

		EVENT_CLASS_TYPE(AppRendered)
		EVENT_CLASS_CATEGORY(LUST_EVENT_CATEGORY_APPLICATION)
	};
}