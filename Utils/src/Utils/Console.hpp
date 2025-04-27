#pragma once

#include "LustUtilsDLLMacro.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"
#include <memory>
#include <utility>

template <typename... Args>
using format_string_t = fmt::format_string<Args...>;

namespace Lust
{
    /**
    * @brief logging utility class
    * @details This class is used for logging
    */
    class LUST_UTILS_API Console
    {
    public:
        /**
        * @brief Initializes the console, must be called before logging
        */
        static void Init();

		/**
		* @brief Ends the console, must be called at the end of the program
		*/
        static void End();

        /**
        * @brief Logs a message to the console, in spdlog trace level, for internal use
        * @param fmt The format string
        * @param args The arguments, to be passed to the format string
        */
        template<typename... Args>
        static inline void CoreDebug(format_string_t<Args...> fmt, Args &&... args)
        {
            s_CoreLogger->trace(fmt, std::forward<Args>(args)...);
        }

        /**
        * @brief Logs a message to the console, in spdlog info level, for internal use
        * @param fmt The format string
        * @param args The arguments, to be passed to the format string
        */
        template<typename... Args>
        static inline void CoreLog(format_string_t<Args...> fmt, Args &&... args)
        {
            s_CoreLogger->info(fmt, std::forward<Args>(args)...);
        }

		/**
		* @brief Logs a message to the console, in spdlog warn level, for internal use
        * @param fmt The format string
        * @param args The arguments, to be passed to the format string
		*/
        template<typename... Args>
        static inline void CoreWarn(format_string_t<Args...> fmt, Args &&... args)
        {
            s_CoreLogger->warn(fmt, std::forward<Args>(args)...);
        }

        /**
        * @brief Logs a message to the console, in spdlog error level, for internal use
        * @param fmt The format string
        * @param args The arguments, to be passed to the format string
        */
        template<typename... Args>
        static inline void CoreError(format_string_t<Args...> fmt, Args &&... args)
        {
            s_CoreLogger->error(fmt, std::forward<Args>(args)...);
        }

		/**
		* @brief Logs a message to the console, in spdlog critical level, for internal use
        * @param fmt The format string
        * @param args The arguments, to be passed to the format string
		*/
        template<typename... Args>
        static inline void CoreCritical(format_string_t<Args...> fmt, Args &&... args)
        {
            s_CoreLogger->critical(fmt, std::forward<Args>(args)...);
        }

        /**
        * @brief Asserts a critical section and logs a message to the console, in spdlog trace level, for internal use
        * @param expression The expression to assert
        * @param fmt The format string
        * @param args The arguments, to be passed to the format string
        * 
        */
        template<typename... Args>
        static inline void CoreAssert(bool expression, format_string_t<Args...> fmt, Args &&... args)
        {
            if (!expression)
                s_CoreLogger->critical(fmt, std::forward<Args>(args)...);
            assert(expression);
        }

        /**
        * @brief Logs a message to the console, in spdlog trace level
        * @param fmt The format string
        * @param args The arguments, to be passed to the format string
        */
        template<typename... Args>
        static inline void Debug(format_string_t<Args...> fmt, Args &&... args)
        {
            s_ClientLogger->trace(fmt, std::forward<Args>(args)...);
        }

        /**
        * @brief Logs a message to the console, in spdlog info level
        * @param fmt The format string
        * @param args The arguments, to be passed to the format string
        */
        template<typename... Args>
        static inline void Log(format_string_t<Args...> fmt, Args &&... args)
        {
            s_ClientLogger->info(fmt, std::forward<Args>(args)...);
        }

        /**
        * @brief Logs a message to the console, in spdlog warn level
        * @param fmt The format string
        * @param args The arguments, to be passed to the format string
        */
        template<typename... Args>
        static inline void Warn(format_string_t<Args...> fmt, Args &&... args)
        {
            s_ClientLogger->warn(fmt, std::forward<Args>(args)...);
        }

        /**
        * @brief Logs a message to the console, in spdlog error level
        * @param fmt The format string
        * @param args The arguments, to be passed to the format string
        */
        template<typename... Args>
        static inline void Error(format_string_t<Args...> fmt, Args &&... args)
        {
            s_ClientLogger->error(fmt, std::forward<Args>(args)...);
        }

		/**
		* @brief Logs a message to the console, in spdlog critical level
        * @param fmt The format string
        * @param args The arguments, to be passed to the format string
		*/
        template<typename... Args>
        static inline void Critical(format_string_t<Args...> fmt, Args &&... args)
        {
            s_ClientLogger->critical(fmt, std::forward<Args>(args)...);
        }

        /**
		* @brief Asserts a critical section and logs a message to the console, in spdlog trace level
        * @param expression The expression to assert
        * @param fmt The format string
        * @param args The arguments, to be passed to the format string
		*/
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