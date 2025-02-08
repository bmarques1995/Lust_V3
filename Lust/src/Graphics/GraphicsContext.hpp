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

	enum GraphicsAPI
	{
		SAMPLE_RENDER_GRAPHICS_API_VK = 1,
#ifdef LUST_USES_WINDOWS
		SAMPLE_RENDER_GRAPHICS_API_D3D12,
#endif //D3D12_GUARD
	};

	class LUST_API GraphicsContext
	{
	public:
		virtual ~GraphicsContext() = default;

		virtual void SetClearColor(float r, float g, float b, float a) = 0;

		virtual uint32_t GetUniformAttachment() const = 0;
		virtual uint32_t GetSmallBufferAttachment() const = 0;
		virtual uint32_t GetFramesInFlight() const = 0;

		virtual void FillRenderPass() = 0;
		virtual void SubmitRenderPass() = 0;
		virtual void ReceiveCommands() = 0;
		virtual void DispatchCommands() = 0;
		virtual void Present() = 0;
		virtual void StageViewportAndScissors() = 0;

		virtual void SetVSync(bool enableVSync) = 0;
		virtual bool IsVSyncEnabled() const = 0;

		virtual void Draw(uint32_t elements) = 0;

		virtual const std::string GetGPUName() = 0;

		virtual void WindowResize(uint32_t width, uint32_t height) = 0;

		static GraphicsContext* Instantiate(const Window* window, uint32_t framesInFlight = 3);
	};
}