#pragma once

#include <string>
#include <chrono>
#include <algorithm>
#include <fstream>

#include <thread>
#include "LustUtilsDLLMacro.hpp"
#include <json/json.h>

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
    
    public:
        Instrumentator();

        void BeginSession(const std::string& name, const std::string& filepath = "results.json");
        void EndSession();
        void WriteProfile(const ProfileResult& result);

        void WriteHeader();

        static Instrumentator* Get();

    private:
        InstrumentationSession* m_CurrentSession;
        Json::Value m_Root;
        std::string m_OutputFilename;
        size_t m_ProfileCount;
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
