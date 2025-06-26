#include "SandCoffin.hpp"

#ifdef LUST_SHARED_LIBS
	#define IMPORT_PREFIX __declspec(dllimport)
#else
	#define IMPORT_PREFIX
#endif


extern IMPORT_PREFIX int lust_entrypoint(int argc, char** argv);

#ifdef LUST_CORE_WIN32_ENTRY

#include <windows.h>
#include <CmdArgs.hpp>

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR pCmdLine, _In_ int nCmdShow) {
    // Function implementation
    int argc;
    std::vector<char*> argv;
    Lust::StartCmdLine(&argc, &argv);
    return lust_entrypoint(argc, argv.data());
}

#else

int main(int argc, char** argv)
{
	return lust_entrypoint(argc, argv);
}

#endif