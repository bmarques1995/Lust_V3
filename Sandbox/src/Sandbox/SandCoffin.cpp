#include "SandCoffin.hpp"
#include "SandCoffin2D.hpp"

#include "Instrumentator.hpp"
#include <windows.h>

Lust::SandCoffin::SandCoffin(int argc, char** argv)
{
	m_App = new QGuiApplication(argc, argv);
	PushLayer(new SandCoffin2D());
	
#ifdef LUST_SANDBOX_WINDOWS
	typedef HWND window_handle_t;
#endif

	window_handle_t externalHwnd = nullptr;

	m_WrappedWindow = new ExampleQWindow(reinterpret_cast<WId>(externalHwnd));
	m_WrappedWindow->show();

	QObject::connect(m_App, &QCoreApplication::aboutToQuit, []() {
#ifdef LUST_SANDBOX_WINDOWS
		OutputDebugStringA("Sample: [Hook] Application is about to quit.");
#endif
	});
}

Lust::SandCoffin::~SandCoffin()
{
	this->DestroyApplication();
	delete m_WrappedWindow;
	delete m_App;
}

void Lust::SandCoffin::ExtraRun()
{
	while (m_Running)
	{
		Lust::InstrumentationTimer timer("Frametime");
		m_App->processEvents();
		m_WrappedWindow->manualUpdate();
		RenderAction();
		timer.Stop();
	}
}