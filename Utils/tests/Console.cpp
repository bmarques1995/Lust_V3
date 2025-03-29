#include <gtest/gtest.h>
#include "Console.hpp"


namespace Lust
{
	TEST(Utils, IsConsoleStarting)
	{
		Console::Init();
		Console::Init();
		Console::CoreLog("Sample Log");
		Console::End();
		ASSERT_TRUE(true);
	}
}