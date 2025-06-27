#include "Editor.hpp"
#include "ThreeWayDialog.hpp"
#include "Console.hpp"

Editor::Editor(int argc, char** argv) :
	Lust::SandCoffin(argc, argv), m_CloseController(&m_SignalRegistered)
{
	m_App = new QApplication(argc, argv);

#ifdef LUST_SANDBOX_WINDOWS
	typedef HWND window_handle_t;
#endif

	m_ConsoleWindow.reset(new QTextEdit(nullptr));
	m_ConsoleWindow->setReadOnly(true);
	int max_lines = 500; // keep the text widget to max 500 lines. remove old lines if needed.
	/*
	* (QTextEdit *qt_text_edit,
                  int max_lines,
                  bool dark_colors = false,
                  bool is_utf8 = false)
	*/
	Lust::Console::RegisterQtLogger(
		std::make_shared<spdlog::sinks::qt_color_sink_mt>(m_ConsoleWindow.get(), max_lines, true, true),
		std::make_shared<spdlog::sinks::qt_color_sink_mt>(m_ConsoleWindow.get(), max_lines, true, true)
	);
	m_ConsoleWindow->show();
	Lust::Console::CoreLog("Sample");

	window_handle_t externalHwnd = std::any_cast<HWND>(m_Window->GetNativePointer());

	m_WrappedWindow.reset(QWindow::fromWinId(reinterpret_cast<WId>(externalHwnd)));
	m_WrappedWindowContainer.reset(new GameContainer(m_WrappedWindow));
}

Editor::~Editor()
{
	m_ConsoleWindow.reset();
	m_WrappedWindowContainer.reset();
	m_WrappedWindow.reset();
	m_App->quit();
	delete m_App;
}

void Editor::ExtraRun()
{
	while (m_Running)
	{
		if (m_SignalRegistered)
		{
			m_SignalRegistered = false;
			CreateDialogBox();
		}
		m_App->processEvents();
		m_WrappedWindowContainer->update();
		GameLoop();
	}
}

bool Editor::ProceedClose()
{
	if (!m_CloseController.IsDialogOpen())
		m_CloseController.ShowCloseDialog();
	return false;
}

void Editor::CreateDialogBox()
{
	auto reply = ThreeWayDialog(nullptr);
	if (reply.exec() == QDialog::Accepted) {
		switch (reply.getResult())
		{
			case ThreeWayDialog::OptionA:
			case ThreeWayDialog::OptionB:
			{ 
				m_Window->EmitClose();
				m_Running = false;
				break;
			}
			case ThreeWayDialog::OptionC:
			default: break;
		}
	}
	m_CloseController.CloseDialog();
}
