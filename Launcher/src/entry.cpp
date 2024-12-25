#include "Sandbox.hpp"

#ifdef LUST_CORE_WIN32_ENTRY

#include <windows.h>

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	Lust::Application* app = new Lust::Sandbox();
	app->Run();
	delete app;
	return 0;
}

#else

int main(int argc, char** argv)
{
	Lust::Application* app = new Lust::Sandbox();
	app->Run();
	delete app;
	return 0;
}

#endif