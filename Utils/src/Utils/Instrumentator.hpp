#pragma once

#include <string>
#include <chrono>
#include <algorithm>
#include <fstream>

#include <thread>
#include "LustUtilsDLLMacro.hpp"

namespace Lust {
    struct LUST_UTILS_API ProfileResult
    {
        std::string Name;
        long long Start, End;
        uint32_t ThreadID;
    };

    struct InstrumentationSession
    {
        std::string Name;
    };

    class LUST_UTILS_API Instrumentator
    {
    private:
        InstrumentationSession* m_CurrentSession;
        std::ofstream m_OutputStream;
        int m_ProfileCount;
    public:
        Instrumentator();

        void BeginSession(const std::string& name, const std::string& filepath = "results.json");
        void EndSession();
        void WriteProfile(const ProfileResult& result);

        void WriteHeader();
        void WriteFooter();

        static Instrumentator* Get();
    };

    class LUST_UTILS_API InstrumentationTimer
    {
    public:
        InstrumentationTimer(const char* name);
        ~InstrumentationTimer();

        void Stop();
    private:
        const char* m_Name;
        std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;
        bool m_Stopped;
    };

}
