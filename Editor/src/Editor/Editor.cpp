#include "Editor.hpp"
#include "ThreeWayDialog.hpp"
#include "Console.hpp"
#include "TranslationCompiler.hpp"
#include "Translator.hpp"

Editor::Editor(int argc, char** argv) :
	Lust::SandCoffin(argc, argv), m_CloseController(&m_SignalRegistered)
{
	m_App = new QApplication(argc, argv);

#ifdef LUST_SANDBOX_WINDOWS
	typedef HWND window_handle_t;
#endif

	TranslationCompiler compiler;
	compiler.Compile("./translations", "./");

	{
		auto language = m_Starter->GetCurrentLanguage();
		m_Translator.reset(new QTranslator());
		auto loaded = m_Translator->load("./translations/dialogbox_pt_br.qm");
		m_Translator->setObjectName("Message");
		m_App->installTranslator(m_Translator.get());
		Lust::Translator::Translate("./translations/sample");
		Lust::Translator::LoadTranslation("./translations/sample.fbd");
	}
	{
		m_ConsoleWindow.reset(new QTextEdit(nullptr));
		m_ConsoleWindow->setReadOnly(true);
		int max_lines = 500;
		Lust::Console::RegisterQtLogger(
			std::make_shared<spdlog::sinks::qt_color_sink_mt>(m_ConsoleWindow.get(), max_lines, true, true),
			std::make_shared<spdlog::sinks::qt_color_sink_mt>(m_ConsoleWindow.get(), max_lines, true, true)
		);
		m_ConsoleWindow->show();
		Lust::Console::CoreLog("Sample");
	}
	window_handle_t externalHwnd = std::any_cast<window_handle_t>(m_Window->GetNativePointer());

	m_WrappedWindow.reset(QWindow::fromWinId(reinterpret_cast<WId>(externalHwnd)));
	m_WrappedWindowContainer.reset(new GameContainer(m_WrappedWindow));
}

Editor::~Editor()
{
	DestroyApplication();
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
		m_ConsoleWindow->update();
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
