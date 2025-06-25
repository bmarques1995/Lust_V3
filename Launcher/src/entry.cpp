#include "SandCoffin.hpp"

#ifdef LUST_SHARED_LIBS
	#define IMPORT_PREFIX __declspec(dllimport)
#else
	#define IMPORT_PREFIX
#endif


extern IMPORT_PREFIX int lust_entrypoint(int argc, char** argv);

#ifdef LUST_CORE_WIN32_ENTRY

#include <windows.h>
#include <shellapi.h>

void startCmdLine(int* out_argc, std::vector<char*>* out_argv);

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR pCmdLine, _In_ int nCmdShow) {
    // Function implementation
    int argc;
    std::vector<char*> argv;
    startCmdLine(&argc, &argv);
    return lust_entrypoint(argc, argv.data());
}

void startCmdLine(int* out_argc, std::vector<char*>* out_argv)
{
    // This function is intentionally left empty.
    // It is used to ensure that the entry point is compatible with the Windows subsystem.
    int argc;
    LPWSTR* argvW = CommandLineToArgvW(GetCommandLineW(), &argc);
    *out_argc = argc;

    std::vector<std::string> utf8Args;
    std::vector<char*> argv;

    for (int i = 0; i < argc; ++i) {
        int len = WideCharToMultiByte(CP_UTF8, 0, argvW[i], -1, nullptr, 0, nullptr, nullptr);
        std::string utf8(len - 1, 0);
        WideCharToMultiByte(CP_UTF8, 0, argvW[i], -1, utf8.data(), len, nullptr, nullptr);
        utf8Args.push_back(utf8);
        argv.push_back(utf8Args.back().data());
    }
    argv.push_back(nullptr);
    *out_argv = argv;
}

#else

int main(int argc, char** argv)
{
	return lust_entrypoint(argc, argv);
}

#endif