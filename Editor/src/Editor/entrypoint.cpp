#include "SandCoffin.hpp"

#ifdef LUST_CORE_WIN32_ENTRY

#include <windows.h>
#include <CmdArgs.hpp>
#include "Editor.hpp"
#include "Instrumentator.hpp"

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR pCmdLine, _In_ int nCmdShow) {
    // Function implementation
    int argc;
    std::vector<char*> argv;
    Lust::StartCmdLine(&argc, &argv);
	Lust::Instrumentator* instrumentator = Lust::Instrumentator::Get();
	instrumentator->BeginSession("Startup", "lust_startup.json");
	Editor* app = new Editor(argc, argv.data());
	instrumentator->EndSession();
	instrumentator->BeginSession("Runtime", "lust_runtime.json");
	app->ExtraRun();
	instrumentator->EndSession();
	instrumentator->BeginSession("Shutdown", "lust_shutdown.json");
	delete app;
	instrumentator->EndSession();
	return 0;
}

#else

int main(int argc, char** argv)
{
    return lust_entrypoint(argc, argv);
}

#endif