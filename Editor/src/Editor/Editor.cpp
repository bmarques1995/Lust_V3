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

	m_WrappedWindow = QWindow::fromWinId(reinterpret_cast<WId>(externalHwnd));
}

Editor::~Editor()
{
	this->DestroyApplication();
	delete m_WrappedWindow;
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
