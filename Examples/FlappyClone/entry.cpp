#include "Controller.hpp"
#include "LustSandboxDLLMacro.hpp"
#include "Console.hpp"

int lust_entrypoint()
{
	Lust::Console::Init();
	Lust::Application* app = new Controller();
	app->Run();
	delete app;
	Lust::Console::End();
	return 0;
}

#ifdef LUST_CORE_WIN32_ENTRY

#include <windows.h>

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	return lust_entrypoint();
}

#else

int main(int argc, char** argv)
{
	return lust_entrypoint();
}

#endif