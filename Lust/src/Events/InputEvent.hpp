#pragma once

#include "Event.hpp"
#include "KeyCodes.hpp"
#include "MouseCodes.hpp"
#include "GamepadCodes.hpp"
#include <sstream>

namespace Lust
{

	class KeyEvent : public Event
	{
	public:
		KeyCode GetKeyCode() const { return m_Keycode; }

		EVENT_CLASS_CATEGORY(LUST_EVENT_CATEGORY_INPUT | LUST_EVENT_CATEGORY_KEYBOARD)
	protected:
		KeyEvent(const KeyCode keycode)
			: m_Keycode(keycode) {
		}

		KeyCode m_Keycode;
	};

	class KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(const KeyCode keycode, bool isRepeat = false)
			: KeyEvent(keycode), m_IsRepeat(isRepeat) {
		}

		bool IsRepeat() const { return m_IsRepeat; }

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

	class KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(const KeyCode keycode)
			: KeyEvent(keycode) {
		}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << m_Keycode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};

	class TextTypedEvent : public Event
	{
	public:
		TextTypedEvent(const char* text) :
			m_Text(text)
		{
		
		}

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

	class MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(const float x, const float y)
			: m_MouseX(x), m_MouseY(y) {
		}

		float GetX() const { return m_MouseX; }
		float GetY() const { return m_MouseY; }

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

	class MouseScrolledEvent : public Event
	{
	public:
		MouseScrolledEvent(const float xOffset, const float yOffset)
			: m_XOffset(xOffset), m_YOffset(yOffset) {
		}

		float GetXOffset() const { return m_XOffset; }
		float GetYOffset() const { return m_YOffset; }

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

	class MouseButtonEvent : public Event
	{
	public:
		MouseCode GetMouseButton() const { return m_Button; }

		EVENT_CLASS_CATEGORY(LUST_EVENT_CATEGORY_INPUT |
			LUST_EVENT_CATEGORY_MOUSE |
			LUST_EVENT_CATEGORY_MOUSE_BUTTON)
	protected:
		MouseButtonEvent(const MouseCode button)
			: m_Button(button) {
		}

		MouseCode m_Button;
	};

	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(const MouseCode button)
			: MouseButtonEvent(button) {
		}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << m_Button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonPressed)
	};

	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(const MouseCode button)
			: MouseButtonEvent(button) {
		}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: " << m_Button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonReleased)
	};

	class GamepadConnectedEvent : public Event
	{
	public:
		GamepadConnectedEvent(const uint32_t joystickNumber, const uint32_t vendor, const uint32_t totalJoysticks) :
			m_JoystickNumber(joystickNumber), m_Vendor(vendor), m_TotalJoysticks(totalJoysticks)
		{
		}
		
		uint32_t GetJoystickNumber() const { return m_JoystickNumber; }
		uint32_t GetVendor() const { return m_Vendor; }
		uint32_t GetTotalJoysticks() const { return m_TotalJoysticks; }

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

	class JoystickInputEvent : public Event
	{
	public:
		KeyCode GetKeyCode() const { return m_Keycode; }
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

	class GamepadKeyPressedEvent : public JoystickInputEvent
	{
	public:
		GamepadKeyPressedEvent(uint32_t joystickNumber, GamepadKeyCode keycode, GamepadKeyValue value):
			m_Value(value), JoystickInputEvent(joystickNumber, keycode)
		{
		
		}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "JoystickKeyPressedEvent: (Keycode: " << m_Keycode << ", Value: " << m_Value << ")";
			return ss.str();
		}

		inline GamepadKeyValue GetValue() const { return m_Value; }
		
		EVENT_CLASS_TYPE(JoystickKeyPressed)
	private:
		GamepadKeyValue m_Value;
	};

	class GamepadKeyReleasedEvent : public JoystickInputEvent
	{
	public:
		GamepadKeyReleasedEvent(uint32_t joystickNumber, GamepadKeyCode keycode, GamepadKeyValue value) :
			m_Value(value), JoystickInputEvent(joystickNumber, keycode)
		{
			
		}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "JoystickKeyReleasedEvent: (Keycode: " << m_Keycode << ", Value: " << m_Value << ")";
			return ss.str();
		}

		inline GamepadKeyCode GetKeycode() const { return m_Keycode; }
		inline GamepadKeyValue GetValue() const { return m_Value; }

		EVENT_CLASS_TYPE(JoystickKeyReleased)
	private:
		GamepadKeyValue m_Value;
	};

	class GamepadAxisMovedEvent : public JoystickInputEvent
	{
	public:
		GamepadAxisMovedEvent(uint32_t joystickNumber, GamepadKeyCode keycode, GamepadKeyValue value) :
			m_Value(value), JoystickInputEvent(joystickNumber, keycode)
		{

		}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "JoystickAxisMovedEvent: (Keycode: " << m_Keycode << ", Value: " << m_Value << ")";
			return ss.str();
		}

		inline GamepadKeyValue GetValue() const { return m_Value; }

		EVENT_CLASS_TYPE(JoystickAxisMoved)
	private:
		GamepadKeyValue m_Value;
	};	
}
