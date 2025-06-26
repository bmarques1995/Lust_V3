#include "CmdArgs.hpp"

#ifdef LUST_USES_WINDOWS

#include <shellapi.h>

LUST_API void Lust::StartCmdLine(int* out_argc, std::vector<char*>* out_argv)
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

#endif
