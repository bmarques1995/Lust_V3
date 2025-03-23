#include <gtest/gtest.h>
#include "FileHandler.hpp"
#ifdef LUST_UTILS_WINDOWS
#include <windows.h>
#endif

namespace Lust
{
	TEST(Utils, IsReadingWritingText)
	{
		std::string msg = "Hello World!\nFor Tests\n";
		FileHandler::WriteTextFile("test.txt", msg);
		std::string readMsg;
		FileHandler::ReadTextFile("test.txt", &readMsg);
		std::remove("test.txt");
		EXPECT_EQ(msg, readMsg);
	}

	TEST(Utils, IsReadingWritingBin)
	{
		std::string msg = "Hello World bin!\nFor Tests\n\0";
		FileHandler::WriteBinFile("test2.txt", (std::byte*)msg.c_str(), msg.size() + 1);
		char* readMsg;
		size_t size;
		FileHandler::ReadBinFile("test2.txt", (std::byte**)&readMsg, &size);
		std::remove("test2.txt");
		std::string strReadMsg = readMsg;
		EXPECT_EQ(msg, strReadMsg);
	}

	TEST(Utils, IsFileExists)
	{
		std::string msg = "Hello World bin!\nFor Tests\n\0";
		FileHandler::WriteBinFile("test3.txt", (std::byte*)msg.c_str(), msg.size() + 1);
		EXPECT_TRUE(FileHandler::FileExists("test3.txt"));
		std::remove("test3.txt");
		EXPECT_FALSE(FileHandler::FileExists("test3.txt"));
	}

	TEST(Utils, ReadFailure)
	{
		std::string readMsg;
		EXPECT_FALSE(FileHandler::ReadTextFile("test.txt", &readMsg));
		char* msg;
		size_t size;
		EXPECT_FALSE(FileHandler::ReadBinFile("test2.txt", (std::byte**)&msg, &size));
	}

	TEST(Utils, WriteFailure)
	{
		std::string msg = "Hello World!\nFor Tests\n";
		FileHandler::WriteTextFile("test4.txt", msg);

#ifdef LUST_UTILS_WINDOWS
		// Set file to read-only
		SetFileAttributesA("test4.txt", FILE_ATTRIBUTE_READONLY);
#endif

		std::string readMsg;
		EXPECT_FALSE(FileHandler::WriteTextFile("test4.txt", msg));
		char binMsg[] = "Sample Message";
		size_t size = sizeof(msg);
		bool res = FileHandler::WriteBinFile("test4.txt", (std::byte*)binMsg, size);
		std::remove("test4.txt");
		EXPECT_FALSE(res);
	}
}