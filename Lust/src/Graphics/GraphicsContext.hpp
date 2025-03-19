#pragma once

#include "LustDLLMacro.hpp"
#include <any>
#include <cstdint>
#include <string>
#include "Window.hpp"
#include "CommonException.hpp"

#include <exception>

namespace Lust
{
	/**
	* @brief Graphics API
	* @param SAMPLE_RENDER_GRAPHICS_API_VK Rendering API Vulkan
	* @param SAMPLE_RENDER_GRAPHICS_API_D3D12 Rendering API Direct3D 12
	*/
	enum GraphicsAPI
	{
		SAMPLE_RENDER_GRAPHICS_API_VK = 1,
#ifdef LUST_USES_WINDOWS
		SAMPLE_RENDER_GRAPHICS_API_D3D12,
#endif //D3D12_GUARD
	};

	/**
	* @brief Interface for graphics context,
	* @details Manages graphics resources, devices, command buffers/lists, RTVs, DSVs, swapchains and so on.
	* Is the entrypoint for all graphics operations
	*/
	class LUST_API GraphicsContext
	{
	public:
		/**
		* @brief Graphics context destructor
		*/
		virtual ~GraphicsContext() = default;

		/**
		* @brief Set clear color
		*/
		virtual void SetClearColor(float r, float g, float b, float a) = 0;

		/**
		* @brief Get uniform attachment
		*/
		virtual uint32_t GetUniformAttachment() const = 0;
		/**
		* @brief Get small buffer attachment
		*/
		virtual uint32_t GetSmallBufferAttachment() const = 0;
		/**
		* @brief Get frames in flight
		*/
		virtual uint32_t GetFramesInFlight() const = 0;

		/**
		* @brief Fill render pass
		* @details Enables the cmd buffer/list
		*/
		virtual void FillRenderPass() = 0;
		/**
		* @brief Submit render pass
		* @details Dispatches the cmd buffer/list
		*/
		virtual void SubmitRenderPass() = 0;
		/**
		* @brief Receive commands
		* @details Enables the cmd buffer/list
		*/
		virtual void ReceiveCommands() = 0;
		/**
		* @brief Dispatch commands
		* @details Dispatches the cmd buffer/list
		*/
		virtual void DispatchCommands() = 0;
		/**
		* @brief Present
		* @details Displays the swapchain image
		*/
		virtual void Present() = 0;
		/**
		* @brief Stage viewport and scissors
		* @details Sets the viewport and scissors to the command buffer/list
		*/
		virtual void StageViewportAndScissors() = 0;

		/**
		* @brief Set vsync
		* @details Enables or disables vsync
		*/
		virtual void SetVSync(bool enableVSync) = 0;
		/**
		* @brief Is vsync enabled
		* @details Returns true if vsync is enabled
		*/
		virtual bool IsVSyncEnabled() const = 0;

		/**
		* @brief Get GPU name
		* @details Returns the name of the GPU
		*/
		virtual const std::string& GetGPUName() = 0;

		/**
		* @brief Resize elements
		* @details Resizes all elements, like the swapchain, RTVs and DSVs
		*/
		virtual void WindowResize(uint32_t width, uint32_t height) = 0;

		/**
		* @brief Instantiate graphics context
		* @details Factory method
		* @param window Window
		* @param framesInFlight Number of frames in flight
		*/
		static GraphicsContext* Instantiate(const Window* window, uint32_t framesInFlight = 3);

	private:
	};
}