#include "SandCoffin.hpp"
#include "SandCoffin2D.hpp"

#include "Instrumentator.hpp"
#include <QMessageBox>
#include <functional>

Lust::SandCoffin::SandCoffin(int argc, char** argv):
	m_CloseController(&m_SignalRegistered)
{
	m_App = new QApplication(argc, argv);
	PushLayer(new SandCoffin2D());
	
#ifdef LUST_SANDBOX_WINDOWS
	typedef HWND window_handle_t;
#endif

	window_handle_t externalHwnd = std::any_cast<HWND>(m_Window->GetNativePointer());

	m_WrappedWindow = QWindow::fromWinId(reinterpret_cast<WId>(externalHwnd));
}

Lust::SandCoffin::~SandCoffin()
{
	this->DestroyApplication();
	delete m_WrappedWindow;
	delete m_App;
}

bool Lust::SandCoffin::ProceedClose()
{
	if (!m_CloseController.IsDialogOpen())
		m_CloseController.ShowCloseDialog();
	return false;
}

void Lust::SandCoffin::ExtraRun()
{
	while (m_Running)
	{
		if (m_SignalRegistered)
		{
			m_SignalRegistered = false;
			CreateDialogBox();
		}
		Lust::InstrumentationTimer timer("Frametime");
		m_App->processEvents();
		RenderAction();
		timer.Stop();
	}
}

void Lust::SandCoffin::CreateDialogBox()
{
	auto reply = QMessageBox::question(nullptr, "Confirm Exit",
		"Are you sure you want to quit?",
		QMessageBox::Yes | QMessageBox::No);

	if (reply == QMessageBox::Yes) {
		m_Window->EmitClose();
		m_Running = false;
		m_CloseController.CloseDialog();
	}
	else {
		m_CloseController.CloseDialog();
	}
}
