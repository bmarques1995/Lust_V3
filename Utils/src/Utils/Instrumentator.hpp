#pragma once

#include <string>
#include <chrono>
#include <algorithm>
#include <fstream>

#include <thread>
#include "LustUtilsDLLMacro.hpp"
#include <json/json.h>

namespace Lust {

    /**
    * @brief A profile result
    * @param Name The name of the result
    * @param Start The start time of the result (in microseconds)
    * @param End The end time of the result (in microseconds)
    * @param ThreadID The thread ID of the result
    */
    struct LUST_UTILS_API ProfileResult
    {
        std::string Name;
        long long Start, End;
        uint32_t ThreadID;
    };

    /**
    * @brief An instrumentation session
    */
    struct InstrumentationSession
    {
        std::string Name;
    };

    /**
    * @brief An instrumentation session, to measure global app times
    */
    class LUST_UTILS_API Instrumentator
    {
    
    public:

        /**
        * @brief Default constructor
        */
        Instrumentator();

        /**
        * @brief Begins a profiling session
        * @param name The name of the session
        * @param filepath The filepath of the results
        */
        void BeginSession(const std::string& name, const std::string& filepath = "results.json");
        /**
        * @brief Ends a profiling session
        */
        void EndSession();
        /**
        * @brief Writes a profile result
        */
        void WriteProfile(const ProfileResult& result);

        /**
        * @brief Writes the header
        */
        void WriteHeader();

        /**
        * @brief Gets the singleton
        */
        static Instrumentator* Get();

    private:
        InstrumentationSession* m_CurrentSession;
        Json::Value m_Root;
        std::string m_OutputFilename;
        size_t m_ProfileCount;
    };

    /**
    * @brief A profiling timer
    * @param m_Name The name of the timer
    * @param m_Stopped If the timer is stopped
    * @param m_StartTimepoint The start time of the timer
    */
    class LUST_UTILS_API InstrumentationTimer
    {
    public:

        /**
        * @brief Default constructor
        * @param name The name of the timer
        */
        InstrumentationTimer(const char* name);
        /**
        * @brief Default destructor
        */
        ~InstrumentationTimer();

		/**
		* @brief Stops the timer
		*/
        void Stop();
    private:
        const char* m_Name;
        std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;
        bool m_Stopped;
    };

}
