#include "Editor.hpp"
#include "ThreeWayDialog.hpp"

Editor::Editor(int argc, char** argv) :
	Lust::SandCoffin(argc, argv), m_CloseController(&m_SignalRegistered)
{
	m_App = new QApplication(argc, argv);

#ifdef LUST_SANDBOX_WINDOWS
	typedef HWND window_handle_t;
#endif

	window_handle_t externalHwnd = std::any_cast<HWND>(m_Window->GetNativePointer());

	m_WrappedWindow.reset(QWindow::fromWinId(reinterpret_cast<WId>(externalHwnd)));
	m_WrappedWindowContainer.reset(new GameContainer(m_WrappedWindow));
}

Editor::~Editor()
{
	this->DestroyApplication();
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
