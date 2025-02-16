#include "SandCoffin.hpp"
#include "LustSandboxDLLMacro.hpp"
LUST_SANDBOX_API int lust_entrypoint()
{
	Lust::Application* app = new Lust::SandCoffin();
	app->Run();
	delete app;
	return 0;
}
