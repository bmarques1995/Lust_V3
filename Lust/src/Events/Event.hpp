#pragma once

#include "LustDLLMacro.hpp"
#include <cstdint>
#include <string>
#include <functional>

namespace Lust
{
	/**
	* @enum EventType
	* @brief Enumeration of event types
	* @param None = 0
	* @param WindowClosed tracks when the window is closed
	* @param WindowResized tracks when the window is resized
	* @param WindowMinimized tracks when the window is minimized
	* @param WindowRestored tracks when the window is restored
	* @param WindowMaximized tracks when the window is maximized
	* @param WindowFocused tracks when the window is focused
	* @param WindowLostFocus tracks when the window is lost focus
	* @param WindowMoved tracks when the window is moved
	* @param SocketConnected tracks when a socket is connected
	* @param SocketDisconnected tracks when a socket is disconnected
	* @param SocketDataReceived tracks when data is received from a socket
	* @param SocketDataSent tracks when data is sent to a socket
	* @param AppTicked tracks when the application is ticked
	* @param AppUpdated tracks when the application is updated
	* @param AppRendered tracks when the application is rendered
	* @param KeyPressed tracks when a key is pressed
	* @param KeyReleased tracks when a key is released
	* @param TextTyped tracks when text is typed
	* @param MouseButtonPressed tracks when a mouse button is pressed
	* @param MouseButtonReleased tracks when a mouse button is released
	* @param MouseMoved tracks when the mouse is moved
	* @param MouseScrolled tracks when the mouse is scrolled
	* @param JoystickConnected tracks when a joystick is connected
	* @param JoystickDisconnected tracks when a joystick is disconnected
	* @param JoystickKeyPressed tracks when a joystick key is pressed
	* @param JoystickKeyReleased tracks when a joystick key is released
	* @param JoystickAxisMoved tracks when a joystick axis is moved
	* @param JoystickHatActivated tracks when a joystick hat is activated 
	*/
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
	
		SocketConnected,
		SocketDisconnected,
		SocketDataReceived,
		SocketDataSent,

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

	/**
	* @brief Enumeration of event categories
	* @param None = 0
	* @param Application event category application
	* @param Input event category input, for keyboard, mouse and joystick
	* @param Keyboard event category keyboard
	* @param Mouse event category mouse
	* @param MouseButton event category mouse button
	* @param Joystick event category joystick
	* @param JoystickButton event category joystick button
	*/
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
		LUST_EVENT_CATEGORY_SOCKET = 0x80,
	};

#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::##type; }\
								virtual EventType GetEventType() const override { return GetStaticType(); }\
								virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual uint32_t GetCategoryFlags() const override { return category; }

	/**
	* @brief Base class for all events
	*/
	class LUST_API Event
	{
		friend class EventDispatcher;
	public:
		/**
		* @brief Returns the type of the event
		*/
		virtual EventType GetEventType() const = 0;
		/**
		* @brief Returns the name of the event
		*/
		virtual const char* GetName() const = 0;
		/**
		* @brief Returns the category flags of the event
		*/
		virtual uint32_t GetCategoryFlags() const = 0;
		/**
		* @brief Returns a string representation of the event
		*/
		virtual std::string ToString() const { return GetName(); }
		
		/**
		* @brief Returns true if the event has been handled
		*/
		bool IsHandled() const { return m_Handled; }

		/**
		* @brief Returns true if the event is in the specified category
		*/
		inline bool IsInCategory(EventCategory category)
		{
			return GetCategoryFlags() & category;
		}
	protected:
		bool m_Handled = false;
	};

	/**
	* @brief Class for dispatching events
	*/
	class LUST_API EventDispatcher
	{
		template<typename T>
		using EventFn = std::function<bool(T&)>;
	public:
		/**
		* @brief Constructor
		* @param event The event to dispatch
		*/
		EventDispatcher(Event& event) :
			m_Event(event)
		{
		}

		/**
		* @brief Dispatches the event to the specified function
		* @param func The function to dispatch the event to
		* @param isGlobal If true, the event will be marked as handled
		*/
		template<typename T, typename F>
		bool Dispatch(const F& func, bool isGlobal)
		{
			if(m_Event.GetEventType() == T::GetStaticType())
			{
				m_Event.m_Handled |= func(static_cast<T&>(m_Event));
				if (isGlobal)
					m_Event.m_Handled = false;
				return true;
			}
			return false;
		}

	private:
		Event& m_Event;
	};

	/**
	* @brief Overload of the << operator for events
	*/
	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}
}