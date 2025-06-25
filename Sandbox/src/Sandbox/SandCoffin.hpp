#pragma once
#include "LustSandboxDLLMacro.hpp"
#include <Application.hpp>
#include <QApplication>
#include <QMessageBox>
#include <QWindow>
#include <QWidget>
#include "CloseController.hpp"

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
		bool ProceedClose() override;
		
	private:
		void CreateDialogBox();

		bool m_SignalRegistered = false;
		CloseController m_CloseController;
		QGuiApplication* m_GuiApp;
		QApplication* m_App;
		QMessageBox* m_MessageBox;
		QWindow* m_WrappedWindow;
		QWidget* m_WrappedWindowContainer;
	};
}
