#pragma once

#include "Event.hpp"
#include <sstream>

namespace Lust {

	class WindowResizedEvent : public Event
	{
	public:
		WindowResizedEvent(unsigned int width, unsigned int height)
			: m_Width(width), m_Height(height) {
		}

		unsigned int GetWidth() const { return m_Width; }
		unsigned int GetHeight() const { return m_Height; }

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

	class WindowClosedEvent : public Event
	{
	public:
		WindowClosedEvent() = default;

		EVENT_CLASS_TYPE(WindowClosed)
		EVENT_CLASS_CATEGORY(LUST_EVENT_CATEGORY_APPLICATION)
	};

	class AppTickEvent : public Event
	{
	public:
		AppTickEvent() = default;

		EVENT_CLASS_TYPE(AppTicked)
		EVENT_CLASS_CATEGORY(LUST_EVENT_CATEGORY_APPLICATION)
	};

	class AppUpdateEvent : public Event
	{
	public:
		AppUpdateEvent() = default;

		EVENT_CLASS_TYPE(AppUpdated)
		EVENT_CLASS_CATEGORY(LUST_EVENT_CATEGORY_APPLICATION)
	};

	class AppRenderEvent : public Event
	{
	public:
		AppRenderEvent() = default;

		EVENT_CLASS_TYPE(AppRendered)
		EVENT_CLASS_CATEGORY(LUST_EVENT_CATEGORY_APPLICATION)
	};
}