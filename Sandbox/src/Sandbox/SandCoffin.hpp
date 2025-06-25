#pragma once
#include "LustSandboxDLLMacro.hpp"
#include <Application.hpp>
#include <QGuiApplication>
#include "ExampleQtWindow.hpp"

namespace Lust
{
	
	class LUST_SANDBOX_API SandCoffin : public Application
	{
	public:
		SandCoffin(int argc, char** argv);
		~SandCoffin();

		/**
		 * @brief Extra run method for the sandbox, can be used to run additional code
		 * @details This method is called after the main loop of the application
		 */
		void ExtraRun();
	private:
		QGuiApplication* m_App;
		ExampleQWindow* m_WrappedWindow;
	};
}
