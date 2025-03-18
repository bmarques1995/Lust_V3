#pragma once

#include "Event.hpp"
#include "KeyCodes.hpp"
#include "MouseCodes.hpp"
#include "GamepadCodes.hpp"
#include <sstream>

namespace Lust
{
	/**
	* @class KeyEvent
	* @brief Base class for key events, all subclasses are in the categories LUST_EVENT_CATEGORY_INPUT | LUST_EVENT_CATEGORY_KEYBOARD
	*/
	class KeyEvent : public Event
	{
	public:
		/**
		* @brief Returns the key code of the event
		*/
		KeyCode GetKeyCode() const { return m_Keycode; }

		EVENT_CLASS_CATEGORY(LUST_EVENT_CATEGORY_INPUT | LUST_EVENT_CATEGORY_KEYBOARD)
	protected:
		/**
		* @brief Constructor
		* @param keycode The key code of the event
		*/
		KeyEvent(const KeyCode keycode)
			: m_Keycode(keycode) {
		}

		KeyCode m_Keycode;
	};

	/**
	* @brief %Event triggered when a key is pressed
	* @details This event is dispatched when a key is pressed. its event type is EventType::KeyPressed
	*/
	class KeyPressedEvent : public KeyEvent
	{
	public:
		/**
		* @brief Constructor
		* @param keycode The key code of the event
		* @param isRepeat True if the key is being held down
		*/
		KeyPressedEvent(const KeyCode keycode, bool isRepeat = false)
			: KeyEvent(keycode), m_IsRepeat(isRepeat) {
		}

		/**
		* @brief Returns true if the key is being held down
		*/
		bool IsRepeat() const { return m_IsRepeat; }

		/**
		* @brief Returns a string representation of the event
		*/
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << m_Keycode << " (repeat = " << m_IsRepeat << ")";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)
	private:
		bool m_IsRepeat;
	};

	/**
	* @brief %Event triggered when a key is released
	* @details This event is dispatched when a key is released. its event type is EventType::KeyReleased
	*/
	class KeyReleasedEvent : public KeyEvent
	{
	public:
		/**
		* @brief Constructor
		* @param keycode The key code of the event
		*/
		KeyReleasedEvent(const KeyCode keycode)
			: KeyEvent(keycode) {
		}

		/**
		* @brief Returns a string representation of the event
		*/
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << m_Keycode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};

	/**
	* @brief %Event triggered when a text is typed
	* @details This event is dispatched when a text is typed. its event type is EventType::TextTyped and its categories are LUST_EVENT_CATEGORY_INPUT | LUST_EVENT_CATEGORY_KEYBOARD
	*/
	class TextTypedEvent : public Event
	{
	public:
		/**
		* @brief Constructor
		* @param text The text that was typed
		*/
		TextTypedEvent(const char* text) :
			m_Text(text)
		{
		
		}

		/**
		* @brief Returns a string representation of the event
		*/
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyTypedEvent: " << m_Text;
			return ss.str();
		}

		EVENT_CLASS_CATEGORY(LUST_EVENT_CATEGORY_INPUT | LUST_EVENT_CATEGORY_KEYBOARD)
		EVENT_CLASS_TYPE(TextTyped)
	private:
		std::string m_Text;
	};

	/**
	* @brief %Event triggered when the mouse is moved
	* @details This event is dispatched when the mouse is moved. its event type is EventType::MouseMoved and its categories are LUST_EVENT_CATEGORY_INPUT | LUST_EVENT_CATEGORY_MOUSE
	*/
	class MouseMovedEvent : public Event
	{
	public:
		/**
		* @brief Constructor
		* @param x The x position of the mouse
		* @param y The y position of the mouse
		*/
		MouseMovedEvent(const float x, const float y)
			: m_MouseX(x), m_MouseY(y) {
		}

		/**
		* @brief Returns the x position of the mouse
		*/
		float GetX() const { return m_MouseX; }
		/**
		* @brief Returns the y position of the mouse
		*/
		float GetY() const { return m_MouseY; }

		/**
		* @brief Returns a string representation of the event
		*/
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent: " << m_MouseX << ", " << m_MouseY;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseMoved)
			EVENT_CLASS_CATEGORY(LUST_EVENT_CATEGORY_INPUT |
				LUST_EVENT_CATEGORY_MOUSE)
	private:
		float m_MouseX, m_MouseY;
	};

	/**
	* @brief %Event triggered when the mouse is scrolled
	* @details This event is dispatched when the mouse is scrolled. its event type is EventType::MouseScrolled and its categories are LUST_EVENT_CATEGORY_INPUT | LUST_EVENT_CATEGORY_MOUSE
	*/
	class MouseScrolledEvent : public Event
	{
	public:
		/**
		* @brief Constructor
		* @param xOffset The x offset of the mouse scroll
		* @param yOffset The y offset of the mouse scroll
		*/
		MouseScrolledEvent(const float xOffset, const float yOffset)
			: m_XOffset(xOffset), m_YOffset(yOffset)
		{
		}

		/**
		* @brief Returns the x offset of the mouse scroll
		*/
		float GetXOffset() const { return m_XOffset; }
		/**
		* @brief Returns the y offset of the mouse scroll
		*/
		float GetYOffset() const { return m_YOffset; }

		/**
		* @brief Returns a string representation of the event
		*/
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseScrolledEvent: " << GetXOffset() << ", " << GetYOffset();
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseScrolled)
			EVENT_CLASS_CATEGORY(LUST_EVENT_CATEGORY_INPUT |
				LUST_EVENT_CATEGORY_MOUSE)
	private:
		float m_XOffset, m_YOffset;
	};

	/**
	* @brief Base class for mouse button events
	* @details This class is used to represent mouse button events, its categories are LUST_EVENT_CATEGORY_INPUT | LUST_EVENT_CATEGORY_MOUSE | LUST_EVENT_CATEGORY_MOUSE_BUTTON
	*/
	class MouseButtonEvent : public Event
	{
	public:

		/**
		* @brief Returns the mouse button that was pressed
		*/
		MouseCode GetMouseButton() const { return m_Button; }

		EVENT_CLASS_CATEGORY(LUST_EVENT_CATEGORY_INPUT |
			LUST_EVENT_CATEGORY_MOUSE |
			LUST_EVENT_CATEGORY_MOUSE_BUTTON)
	protected:
		/**
		* @brief Constructor
		* @param button The mouse button that was pressed
		*/
		MouseButtonEvent(const MouseCode button)
			: m_Button(button) {
		}

		MouseCode m_Button;
	};

	/**
	* @brief %Event triggered when a mouse button is pressed
	* @details This event is dispatched when a mouse button is pressed. its event type is EventType::MouseButtonPressed
	*/
	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		/**
		* @brief Constructor
		* @param button The mouse button that was pressed
		*/
		MouseButtonPressedEvent(const MouseCode button)
			: MouseButtonEvent(button) {
		}

		/**
		* @brief Returns a string representation of the event
		*/
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << m_Button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonPressed)
	};

	/**
	* @brief %Event triggered when a mouse button is released
	* @details This event is dispatched when a mouse button is released. its event type is EventType::MouseButtonReleased
	*/
	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		/**
		* @brief Constructor
		* @param button The mouse button that was released
		*/
		MouseButtonReleasedEvent(const MouseCode button)
			: MouseButtonEvent(button) {
		}

		/**
		* @brief Returns a string representation of the event
		*/
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: " << m_Button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonReleased)
	};

	/**
	* @brief %Event triggered when a gamepad is connected
	* @details This event is dispatched when a gamepad is connected. its event type is EventType::JoystickConnected and its categories are LUST_EVENT_CATEGORY_INPUT | LUST_EVENT_CATEGORY_JOYSTICK
	*/
	class GamepadConnectedEvent : public Event
	{
	public:
		/**
		* @brief Constructor
		* @param joystickNumber The number of the joystick that was connected
		* @param vendor The vendor of the joystick that was connected, e.g. Microsoft, Playstation, etc.
		* @param totalJoysticks The total number of joysticks that are connected
		*/
		GamepadConnectedEvent(const uint32_t joystickNumber, const uint32_t vendor, const uint32_t totalJoysticks) :
			m_JoystickNumber(joystickNumber), m_Vendor(vendor), m_TotalJoysticks(totalJoysticks)
		{
		}
		
		/**
		* @brief Returns the number of the joystick that was connected
		*/
		uint32_t GetJoystickNumber() const { return m_JoystickNumber; }
		/**
		* @brief Returns the vendor of the joystick that was connected
		*/
		uint32_t GetVendor() const { return m_Vendor; }
		/**
		* @brief Returns the total number of joysticks that are connected
		*/
		uint32_t GetTotalJoysticks() const { return m_TotalJoysticks; }

		/**
		* @brief Returns a string representation of the event
		*/
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "JoystickConnectedEvent: " << m_JoystickNumber;
			return ss.str();
		}

		EVENT_CLASS_TYPE(JoystickConnected)
		EVENT_CLASS_CATEGORY(LUST_EVENT_CATEGORY_INPUT |
			LUST_EVENT_CATEGORY_JOYSTICK)
	private:
		uint32_t m_JoystickNumber;
		uint32_t m_Vendor;
		uint32_t m_TotalJoysticks;
	};

	/**
	* @brief %Event triggered when a gamepad is disconnected
	* @details This event is dispatched when a gamepad is disconnected. its event type is EventType::JoystickDisconnected and its categories are
	* LUST_EVENT_CATEGORY_INPUT | LUST_EVENT_CATEGORY_JOYSTICK
	*/
	class GamepadDisconnectedEvent : public Event
	{
	public:
		GamepadDisconnectedEvent(const uint32_t joystickNumber, const uint32_t vendor, const uint32_t remainingJoysticks) :
			m_JoystickNumber(joystickNumber), m_Vendor(vendor), m_RemainingJoysticks(remainingJoysticks)
		{
		}

		uint32_t GetJoystickNumber() const { return m_JoystickNumber; }
		uint32_t GetVendor() const { return m_Vendor; }
		uint32_t GetRemainingJoysticks() const { return m_RemainingJoysticks; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "JoystickConnectedEvent: " << m_JoystickNumber;
			return ss.str();
		}

		EVENT_CLASS_TYPE(JoystickDisconnected)
		EVENT_CLASS_CATEGORY(LUST_EVENT_CATEGORY_INPUT |
			LUST_EVENT_CATEGORY_JOYSTICK)
	private:
		uint32_t m_JoystickNumber;
		uint32_t m_Vendor;
		uint32_t m_RemainingJoysticks;
	};

	/**
	* @brief Base class for gamepad input events
	* @details This event is dispatched when a gamepad button is pressed. Its categories are
	* LUST_EVENT_CATEGORY_INPUT | LUST_EVENT_CATEGORY_JOYSTICK | LUST_EVENT_CATEGORY_JOYSTICK_BUTTON
	*/
	class JoystickInputEvent : public Event
	{
	public:
		/**
		* @brief Returns the key code of the event
		*/
		KeyCode GetKeyCode() const { return m_Keycode; }
		/**
		* @brief Returns the number of the joystick that sent the event
		*/
		uint32_t GetJoystickNumber() const { return m_JoystickNumber; }
		EVENT_CLASS_CATEGORY(LUST_EVENT_CATEGORY_INPUT |
			LUST_EVENT_CATEGORY_JOYSTICK |
			LUST_EVENT_CATEGORY_JOYSTICK_BUTTON)
	protected:
		JoystickInputEvent(uint32_t joystickNumber, const KeyCode keycode)
			: m_Keycode(keycode), m_JoystickNumber(joystickNumber)
		{
		}

		GamepadKeyCode m_Keycode;
		uint32_t m_JoystickNumber;
	};

	/**
	* @brief %Event triggered when a gamepad button is pressed
	* @details This event is dispatched when a gamepad button is pressed. its event type is EventType::GamepadKeyPressed
	*/
	class GamepadKeyPressedEvent : public JoystickInputEvent
	{
	public:
		/**
		* @brief Constructor
		* @param joystickNumber The number of the joystick that sent the event
		* @param keycode The key code of the event
		* @param value The value of the event, for analog buttons -32768 to 32767 for digital buttons 0 or 1
		*/
		GamepadKeyPressedEvent(uint32_t joystickNumber, GamepadKeyCode keycode, GamepadKeyValue value):
			m_Value(value), JoystickInputEvent(joystickNumber, keycode)
		{
		
		}

		/**
		* @brief Returns a string representation of the event
		*/
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "JoystickKeyPressedEvent: (Keycode: " << m_Keycode << ", Value: " << m_Value << ")";
			return ss.str();
		}

		/**
		* @brief Returns the value of the event
		*/
		inline GamepadKeyValue GetValue() const { return m_Value; }
		
		EVENT_CLASS_TYPE(JoystickKeyPressed)
	private:
		GamepadKeyValue m_Value;
	};

	/**
	* @brief %Event triggered when a gamepad button is released
	* @details This event is dispatched when a gamepad button is released. its event type is EventType::GamepadKeyReleased
	*/
	class GamepadKeyReleasedEvent : public JoystickInputEvent
	{
	public:
		/**
		* @brief Constructor
		* @param joystickNumber The number of the joystick that sent the event
		* @param keycode The key code of the event
		* @param value The value of the event, for analog buttons -32768 to 32767 for digital buttons 0 or 1
		*/
		GamepadKeyReleasedEvent(uint32_t joystickNumber, GamepadKeyCode keycode, GamepadKeyValue value) :
			m_Value(value), JoystickInputEvent(joystickNumber, keycode)
		{
			
		}

		/**
		* @brief Returns a string representation of the event
		*/
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "JoystickKeyReleasedEvent: (Keycode: " << m_Keycode << ", Value: " << m_Value << ")";
			return ss.str();
		}

		/**
		* @brief Returns the value of the event
		*/
		inline GamepadKeyCode GetKeycode() const { return m_Keycode; }
		/**
		* @brief Returns the value of the event
		*/
		inline GamepadKeyValue GetValue() const { return m_Value; }

		EVENT_CLASS_TYPE(JoystickKeyReleased)
	private:
		GamepadKeyValue m_Value;
	};

	/**
	* @brief %Event triggered when a gamepad axis is moved
	* @details This event is dispatched when a gamepad axis is moved. its event type is EventType::GamepadAxisMoved
	*/
	class GamepadAxisMovedEvent : public JoystickInputEvent
	{
	public:
		/**
		* @brief Constructor
		* @param joystickNumber The number of the joystick that sent the event
		* @param keycode The key code of the event
		* @param value The value of the key, for axises, each floats between -32768 to 32767
		*/
		GamepadAxisMovedEvent(uint32_t joystickNumber, GamepadKeyCode keycode, GamepadKeyValue value) :
			m_Value(value), JoystickInputEvent(joystickNumber, keycode)
		{

		}

		/**
		* @brief Returns a string representation of the event
		*/
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "JoystickAxisMovedEvent: (Keycode: " << m_Keycode << ", Value: " << m_Value << ")";
			return ss.str();
		}

		/**
		* @brief Returns the value of the event
		*/
		inline GamepadKeyValue GetValue() const { return m_Value; }

		EVENT_CLASS_TYPE(JoystickAxisMoved)
	private:
		GamepadKeyValue m_Value;
	};
}
