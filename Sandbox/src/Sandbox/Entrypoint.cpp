#include "SandCoffin.hpp"
#include "LustSandboxDLLMacro.hpp"
#include "Instrumentator.hpp"

LUST_SANDBOX_API int lust_entrypoint(int argc, char** argv)
{
	Lust::Instrumentator* instrumentator = Lust::Instrumentator::Get();
	instrumentator->BeginSession("Startup", "lust_startup.json");
	Lust::SandCoffin* app = new Lust::SandCoffin(argc, argv);
	instrumentator->EndSession();
	instrumentator->BeginSession("Runtime", "lust_runtime.json");
	app->ExtraRun();
	instrumentator->EndSession();
	instrumentator->BeginSession("Shutdown", "lust_shutdown.json");
	delete app;
	instrumentator->EndSession();
	return 0;
}
