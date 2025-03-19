#pragma once

#include "LustDLLMacro.hpp"
#include <utility>
#include "KeyCodes.hpp"
#include "MouseCodes.hpp"
#include "GamepadCodes.hpp"

namespace Lust
{
	/**
	* @brief Interface representing input polling
	*/
	class LUST_API Input
	{
	public:
		/**
		* @brief Get if a key is pressed
		* @param keycode The keycode
		* @return True if the key is pressed, false otherwise
		*/
		static bool IsKeyPressed(KeyCode keycode) { return s_Instance->IsKeyPressedImpl(keycode); }
		/**
		* @brief Get if a mouse button is pressed
		* @param button The mouse button
		* @return True if the mouse button is pressed, false otherwise
		*/
		static bool IsMouseButtonPressed(MouseCode button) { return s_Instance->IsMouseButtonPressedImpl(button); }
		/**
		* @brief Get the mouse position, first: x, second: y
		*/
		static std::pair<float, float> GetMousePosition() { return s_Instance->GetMousePositionImpl(); }
		/**
		* @brief Get if a gamepad button is pressed
		*/
		static bool IsGamepadKeyPressed(GamepadKeyCode button, uint32_t player = 1) { return s_Instance->IsGamepadKeyPressedImpl(button, player); }
		/**
		* @brief Get a gamepad axis
		*/
		static int16_t GetGamepadAxis(GamepadAxisCode axis, uint32_t player = 1) { return s_Instance->GetGamepadAxisImpl(axis, player); }

		/**
		* @brief Set the gamepad lower dead zone, where the lower sets the minimum module to be considered pressed
		*/
		static void SetGamepadLowerDeadZone(float deadZonePercent) { s_GamepadLowerDeadZonePercent = deadZonePercent; }
		/**
		* @brief Get the gamepad lower dead zone
		*/
		static float GetGamepadLowerDeadZone() { return s_GamepadLowerDeadZonePercent; }
		/**
		* @brief Set the gamepad upper dead zone
		*/
		static void SetGamepadUpperDeadZone(float deadZonePercent) { s_GamepadUpperDeadZonePercent = deadZonePercent; }
		/**
		* @brief Get the gamepad upper dead zone*
		*/
		static float GetGamepadUpperDeadZone() { return s_GamepadUpperDeadZonePercent; }

		/**
		* @brief Create an Input instance
		* @return The Input instance
		*/
		static Input* CreateInstance();

	protected:
		/**
		* @brief Get if a key is pressed, used by the Input instance
		*/
		virtual bool IsKeyPressedImpl(KeyCode keycode) = 0;
		/**
		* @brief Get if a mouse button is pressed
		*/
		virtual bool IsMouseButtonPressedImpl(MouseCode button) = 0;
		/**
		* @brief Get the mouse position
		*/
		virtual std::pair<float, float> GetMousePositionImpl() = 0;

		/**
		* @brief Get if a gamepad button is pressed
		*/
		virtual bool IsGamepadKeyPressedImpl(GamepadKeyCode button, uint32_t player = 1) = 0;
		/**
		* @brief Get a gamepad axis
		*/
		virtual int16_t GetGamepadAxisImpl(GamepadAxisCode axis, uint32_t player = 1) = 0;
	private:
		static Input* s_Instance;
		static float s_GamepadLowerDeadZonePercent;
		static float s_GamepadUpperDeadZonePercent;
	};
}