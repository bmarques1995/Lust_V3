#include <gtest/gtest.h>
#include "FileHandler.hpp"

namespace Lust
{
	TEST(Utils, IsReadingWritingText)
	{
		std::string msg = "Hello World!\nFor Tests\n";
		FileHandler::WriteTextFile("test.txt", msg);
		std::string readMsg;
		FileHandler::ReadTextFile("test.txt", &readMsg);
		EXPECT_EQ(msg, readMsg);
	}
}