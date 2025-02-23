#pragma once

#include "LustUtilsDLLMacro.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"
#include <memory>

template <typename... Args>
using format_string_t = fmt::format_string<Args...>;

namespace Lust
{
    class LUST_UTILS_API Console
    {
    public:
        static void Init();
        static void End();

        template<typename... Args>
        static inline void CoreDebug(format_string_t<Args...> fmt, Args &&... args)
        {
            s_CoreLogger->trace(fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static inline void CoreLog(format_string_t<Args...> fmt, Args &&... args)
        {
            s_CoreLogger->info(fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static inline void CoreWarn(format_string_t<Args...> fmt, Args &&... args)
        {
            s_CoreLogger->warn(fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static inline void CoreError(format_string_t<Args...> fmt, Args &&... args)
        {
            s_CoreLogger->error(fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static inline void CoreCritical(format_string_t<Args...> fmt, Args &&... args)
        {
            s_CoreLogger->critical(fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static inline void CoreAssert(bool expression, format_string_t<Args...> fmt, Args &&... args)
        {
            if (!expression)
                s_CoreLogger->critical(fmt, std::forward<Args>(args)...);
            assert(expression);
        }

        template<typename... Args>
        static inline void Debug(format_string_t<Args...> fmt, Args &&... args)
        {
            s_ClientLogger->trace(fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static inline void Log(format_string_t<Args...> fmt, Args &&... args)
        {
            s_ClientLogger->info(fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static inline void Warn(format_string_t<Args...> fmt, Args &&... args)
        {
            s_ClientLogger->warn(fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static inline void Error(format_string_t<Args...> fmt, Args &&... args)
        {
            s_ClientLogger->error(fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static inline void Critical(format_string_t<Args...> fmt, Args &&... args)
        {
            s_ClientLogger->critical(fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static inline void Assert(bool expression, format_string_t<Args...> fmt, Args &&... args)
        {
			if (!expression)
                s_ClientLogger->critical(fmt, std::forward<Args>(args)...);
            assert(expression);
        }

    private:
        static std::shared_ptr<spdlog::logger> s_CoreLogger;
        static std::shared_ptr<spdlog::logger> s_ClientLogger;
    };
}