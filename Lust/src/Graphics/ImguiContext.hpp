#pragma once

#include "GraphicsContext.hpp"
#include <memory>
#include "LustDLLMacro.hpp"

namespace Lust
{
	class LUST_API ImguiContext
	{
	public:
		virtual ~ImguiContext() = default;

		static void StartImgui();
		static void EndImgui();
		static void StartFrame();
		static void EndFrame();

		virtual void ReceiveInput() = 0;
		virtual void DispatchInput() = 0;

		static ImguiContext* Instantiate(const GraphicsContext* graphicsContext);
	};
}