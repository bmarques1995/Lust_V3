#pragma once

#include <cstdint>
#include <string>
#include <functional>

namespace Lust
{
	enum class EventType
	{
		None = 0,
	
		WindowClosed,
		WindowResized,
		WindowMinimized,
		WindowRestored,
		WindowMaximized,
		WindowFocused,
		WindowLostFocus,
		WindowMoved,
	
		AppTicked,
		AppUpdated,
		AppRendered,

		KeyPressed,
		KeyReleased,
		TextTyped,

		MouseButtonPressed,
		MouseButtonReleased,
		MouseMoved,
		MouseScrolled,

		JoystickConnected,
		JoystickDisconnected,
		JoystickKeyPressed,
		JoystickKeyReleased,
		JoystickAxisMoved, //For Analogics and triggers
		JoystickHatActivated
	};

	enum EventCategory
	{
		LUST_EVENT_CATEGORY_NONE = 0x0,
		LUST_EVENT_CATEGORY_APPLICATION = 0x1,
		LUST_EVENT_CATEGORY_INPUT = 0x2,
		LUST_EVENT_CATEGORY_KEYBOARD = 0x4,
		LUST_EVENT_CATEGORY_MOUSE = 0x8,
		LUST_EVENT_CATEGORY_MOUSE_BUTTON = 0x10,
		LUST_EVENT_CATEGORY_JOYSTICK = 0x20,
		LUST_EVENT_CATEGORY_JOYSTICK_BUTTON = 0x40,
	};

#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::##type; }\
								virtual EventType GetEventType() const override { return GetStaticType(); }\
								virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual uint32_t GetCategoryFlags() const override { return category; }

	class LUST_API Event
	{
		friend class EventDispatcher;
	public:
		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual uint32_t GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }
		
		inline bool IsInCategory(EventCategory category)
		{
			return GetCategoryFlags() & category;
		}
	protected:
		bool m_Handled = false;
	};

	class LUST_API EventDispatcher
	{
		template<typename T>
		using EventFn = std::function<bool(T&)>;
	public:
		EventDispatcher(Event& event) :
			m_Event(event)
		{
		}

		template<typename T, typename F>
		bool Dispatch(const F& func)
		{
			if(m_Event.GetEventType() == T::GetStaticType())
			{
				m_Event.m_Handled |= func(static_cast<T&>(m_Event));
				return true;
			}
			return false;
		}

	private:
		Event& m_Event;
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}
}