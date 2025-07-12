#include "Console.hpp"

#include <chrono>
#include <spdlog/sinks/rotating_file_sink.h>


#ifdef LUST_UTILS_WINDOWS
#include <spdlog/sinks/msvc_sink.h>
#endif
#if (!defined LUST_UTILS_WINDOWED)
#include <spdlog/sinks/stdout_color_sinks.h>
#endif

std::shared_ptr<spdlog::logger> Lust::Console::s_CoreLogger;
std::shared_ptr<spdlog::logger> Lust::Console::s_ClientLogger;
std::shared_ptr<spdlog::sinks::dup_filter_sink_mt> Lust::Console::s_ClientDupFilter = std::make_shared<spdlog::sinks::dup_filter_sink_mt>(std::chrono::seconds(5));
std::shared_ptr<spdlog::sinks::dup_filter_sink_mt> Lust::Console::s_CoreDupFilter = std::make_shared<spdlog::sinks::dup_filter_sink_mt>(std::chrono::seconds(5));

void Lust::Console::Init()
{
	if((s_ClientLogger.get() != nullptr ) && (s_CoreLogger.get() != nullptr))
		return;

	std::vector<spdlog::sink_ptr> clientLogSinks;
	std::vector<spdlog::sink_ptr> coreLogSinks;

#ifdef LUST_UTILS_WINDOWS
	clientLogSinks.push_back(std::make_shared<spdlog::sinks::msvc_sink_mt>());
	coreLogSinks.push_back(std::make_shared<spdlog::sinks::msvc_sink_mt>());
#endif
#if (!defined LUST_UTILS_WINDOWED)
	clientLogSinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
	coreLogSinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
#endif

	clientLogSinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>("client.log", 1024 * 1024, 5, false));
	coreLogSinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>("core.log", 1024 * 1024, 5, false));

	for (auto it = clientLogSinks.begin(); it != clientLogSinks.end(); it++)
	{
		(*it)->set_pattern("%^[%T][%l] %n: %v%$");
		s_ClientDupFilter->add_sink(*it);
	}

	for (auto it = coreLogSinks.begin(); it != coreLogSinks.end(); it++)
	{
		(*it)->set_pattern("%^[%T][%l] %n: %v%$");
		s_CoreDupFilter->add_sink(*it);
	}

	CreateLoggers();
}

void Lust::Console::End()
{
	spdlog::drop_all();
	s_ClientLogger.reset();
	s_CoreLogger.reset();
}

inline void Lust::Console::RegisterQtLogger(std::shared_ptr<spdlog::sinks::base_sink<std::mutex>> core_logger, std::shared_ptr<spdlog::sinks::base_sink<std::mutex>> client_logger)
{
	End();
	core_logger->set_pattern("%^[%T][%l] %n: %v%$");
	s_CoreDupFilter->add_sink(core_logger);
	client_logger->set_pattern("%^[%T][%l] %n: %v%$");
	s_ClientDupFilter->add_sink(client_logger);
	CreateLoggers();
}

void Lust::Console::CreateLoggers()
{
	s_CoreLogger.reset(new spdlog::logger("Render", s_CoreDupFilter));
	spdlog::register_logger(s_CoreLogger);
	s_CoreLogger->set_level(spdlog::level::trace);
	s_CoreLogger->flush_on(spdlog::level::trace);

	s_ClientLogger.reset(new spdlog::logger("Client", s_ClientDupFilter));
	spdlog::register_logger(s_ClientLogger);
	s_ClientLogger->set_level(spdlog::level::trace);
	s_ClientLogger->flush_on(spdlog::level::trace);
}