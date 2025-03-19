#pragma once

#include "LustDLLMacro.hpp"
#include "Window.hpp"
#include <memory>

namespace Lust
{
	/**
	* @brief Interface representing a desktop system based %Window
	* @details Used to handle imgui in the window side
	*/
	class LUST_API ImguiWindowController
	{
	public:
		/**
		* @brief Virtual destructor
		*/
		virtual ~ImguiWindowController() = default;

		/**
		* @brief Receive input
		*/
		virtual void ReceiveInput() = 0;

		/**
		* @brief Instantiate a window controller
		* @param window The window
		* @return The window controller
		*/
		static ImguiWindowController* Instantiate(const Window* window);
	};
}