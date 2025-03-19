#pragma once

#include "GraphicsContext.hpp"
#include <memory>
#include "LustDLLMacro.hpp"

namespace Lust
{
	/**
	* @brief Interface for a imgui context
	* @details Handles imgui operations, such as input
	*/
	class LUST_API ImguiContext
	{
	public:
		/**
		* @brief Imgui context destructor
		*/
		virtual ~ImguiContext() = default;

		/**
		* @brief Start imgui
		*/
		static void StartImgui();
		/**
		* @brief End imgui
		*/
		static void EndImgui();
		/**
		* @brief Start frame
		*/
		static void StartFrame();
		/**
		* @brief End frame
		*/
		static void EndFrame();

		/**
		* @brief Receive input
		* @details Handles ImGui calls, with API data
		*/
		virtual void ReceiveInput() = 0;
		/**
		* @brief Dispatch input
		* @details Dispatches ImGui calls
		*/
		virtual void DispatchInput() = 0;

		/**
		* @brief Instantiate imgui context
		* @param graphicsContext Graphics context
		* @details Factory method for imgui context
		*/
		static ImguiContext* Instantiate(const GraphicsContext* graphicsContext);
	};
}