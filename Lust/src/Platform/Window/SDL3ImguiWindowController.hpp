#pragma once

#include "LustDLLMacro.hpp"
#include "ImguiWindowController.hpp"
#include "SDL3Window.hpp"

namespace Lust
{
	class LUST_API SDL3ImguiWindowController : public ImguiWindowController
	{
	public:
		SDL3ImguiWindowController(const std::shared_ptr<SDL3Window>* window);
		~SDL3ImguiWindowController();

		void ReceiveInput() override;
	private:
		const std::shared_ptr<SDL3Window>* m_Window;
	};
}