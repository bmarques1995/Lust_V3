#include "SandCoffin.hpp"

#ifdef LUST_SHARED_LIBS
	#define IMPORT_PREFIX __declspec(dllimport)
#else
	#define IMPORT_PREFIX
#endif


extern IMPORT_PREFIX int lust_entrypoint();

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