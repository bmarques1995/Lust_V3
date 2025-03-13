#include "SandCoffin.hpp"
#include "LustSandboxDLLMacro.hpp"
#include "Instrumentator.hpp"

LUST_SANDBOX_API int lust_entrypoint()
{
	Lust::Instrumentator* instrumentator = Lust::Instrumentator::Get();
	instrumentator->BeginSession("Startup", "lust_startup.json");
	Lust::Application* app = new Lust::SandCoffin();
	instrumentator->EndSession();
	instrumentator->BeginSession("Runtime", "lust_runtime.json");
	app->Run();
	instrumentator->EndSession();
	instrumentator->BeginSession("Shutdown", "lust_shutdown.json");
	delete app;
	instrumentator->EndSession();
	return 0;
}
