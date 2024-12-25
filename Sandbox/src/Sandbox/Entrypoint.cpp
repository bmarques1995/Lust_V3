#include "Entrypoint.hpp"
#include "Sandbox.hpp"

int main(int argc, char** argv)
{
	Lust::Application* app = new Lust::Sandbox();
	app->Run();
	delete app;
	return 0;
}
