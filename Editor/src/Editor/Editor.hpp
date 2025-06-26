#pragma once

#include "SandCoffin.hpp"
#include <Application.hpp>
#include <QApplication>
#include <QMessageBox>
#include <QWindow>
#include <QWidget>
#include "CloseController.hpp"

class Editor : public Lust::SandCoffin
{
public:
	Editor(int argc, char** argv);
	~Editor();

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
	QApplication* m_App;
	QWindow* m_WrappedWindow;
};