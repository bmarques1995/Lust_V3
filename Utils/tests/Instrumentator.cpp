#include <gtest/gtest.h>
#include "Instrumentator.hpp"
#include "FileHandler.hpp"
#include <thread>

namespace Lust
{
	void expensiveOperation()
	{
		InstrumentationTimer timer("expensiveOperation");
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	TEST(Utils, IsInstrumentatorRegisterWorking)
	{
		auto instrumentator = Instrumentator::Get();
		instrumentator->BeginSession("test", "sample.json");
		ProfileResult result = {"Tester", 8, 64, 2};
		instrumentator->WriteProfile(result);
		instrumentator->EndSession();
		Json::Value root;
		Json::Reader reader;
		std::string jsonResult;
		FileHandler::ReadTextFile("sample.json", &jsonResult);
		reader.parse(jsonResult, root);
		Json::Value element = root["traceEvents"][0];
		ASSERT_EQ(element["cat"].asString(), "function");
		ASSERT_EQ(element["dur"].asInt(), 56);
		ASSERT_EQ(element["name"].asString(), "Tester");
		ASSERT_EQ(element["ph"].asString(), "X");
		ASSERT_EQ(element["pid"].asInt(), 0);
		ASSERT_EQ(element["tid"].asInt(), 2);
		ASSERT_EQ(element["ts"].asInt(), 8);
		std::remove("sample.json");
	}

	TEST(Utils, IsInstrumentatorRegisterWorkingWithTimer)
	{
		auto instrumentator = Instrumentator::Get();
		instrumentator->BeginSession("test", "sample2.json");
		expensiveOperation();
		instrumentator->EndSession();
		ASSERT_TRUE(FileHandler::FileExists("sample2.json"));
		std::remove("sample2.json");
	}
}
