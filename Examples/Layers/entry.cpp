#include "Controller.hpp"
#include "LustSandboxDLLMacro.hpp"

int lust_entrypoint()
{
	Lust::Application* app = new Controller();
	app->Run();
	delete app;
	return 0;
}

int main()
{ 
	return lust_entrypoint(); 
}