#pragma once

#include "LustDLLMacro.hpp"
#include "Window.hpp"
#include <memory>

namespace Lust
{
	class LUST_API ImguiWindowController
	{
	public:
		virtual ~ImguiWindowController() = default;

		virtual void ReceiveInput() = 0;

		static ImguiWindowController* Instantiate(const Window* window);
	};
}