#pragma once

#include "SandCoffin.hpp"
#include <Application.hpp>
#include <QApplication>
#include <QMessageBox>
#include <QWindow>
#include <QDockWidget>
#include "CloseController.hpp"
#include "GameContainer.hpp"
#include <spdlog/sinks/qt_sinks.h>
#include "Translation.hpp"

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
	Lust::Translation m_Translation;
	std::shared_ptr<QTranslator> m_Translator;
	std::shared_ptr<QWindow> m_WrappedWindow;
	std::shared_ptr<QTextEdit> m_ConsoleWindow;
	std::shared_ptr<GameContainer> m_WrappedWindowContainer;
};