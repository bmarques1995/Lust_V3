#include <gtest/gtest.h>
#include "Image.hpp"
#include "FileHandler.hpp"
#ifdef LUST_UTILS_WINDOWS
#include <windows.h>
#endif

namespace Lust
{
	TEST(Utils, IsPNGImageGenerated)
	{
		uint32_t pixel = 0xFF0000FF;
		uint8_t* buffer;
		size_t bufferSize;
		Image::CastBMPToPNG((unsigned char*)&pixel, 1, 1, &buffer, &bufferSize, false);
		FileHandler::WriteBinFile("test.png", (std::byte*)buffer, bufferSize);
		std::shared_ptr<Image> img;
		img.reset(Image::CreateImage("test.png"));
		std::remove("test.png");
		EXPECT_EQ(img->GetHeight(), 1);
		EXPECT_EQ(img->GetWidth(), 1);
		EXPECT_EQ(img->GetChannels(), 4);
		EXPECT_EQ(img->GetMips(), 1);
		EXPECT_EQ(img->GetImageAlignment(), ImageAlignment::COMPENSED);
		EXPECT_FALSE(img->GetRawPointer() == nullptr);
	}

	TEST(Utils, IsJPEGImageGenerated)
	{
		uint32_t pixel = 0xFF0000FF;
		uint8_t* buffer;
		size_t bufferSize;
		Image::CastBMPToJPEG((unsigned char*)&pixel, 1, 1, &buffer, &bufferSize, false);
		FileHandler::WriteBinFile("test.jpeg", (std::byte*)buffer, bufferSize);
		std::shared_ptr<Image> img;
		img.reset(Image::CreateImage("test.jpeg"));
		std::remove("test.jpeg");
		EXPECT_EQ(img->GetHeight(), 1);
		EXPECT_EQ(img->GetWidth(), 1);
		EXPECT_EQ(img->GetChannels(), 4);
		EXPECT_EQ(img->GetMips(), 1);
		EXPECT_EQ(img->GetImageAlignment(), ImageAlignment::COMPENSED);
		EXPECT_FALSE(img->GetRawPointer() == nullptr);
	}

	TEST(Utils, IsPNGImageGeneratedUsingMemory)
	{
		uint32_t pixel = 0xFF0000FF;
		uint8_t* buffer;
		size_t bufferSize;
		Image::CastBMPToPNG((unsigned char*)&pixel, 1, 1, &buffer, &bufferSize, false);
		std::shared_ptr<Image> img;
		img.reset(Image::CreateImage((std::byte*)buffer, bufferSize, ImageFormat::PNG));
		EXPECT_EQ(img->GetHeight(), 1);
		EXPECT_EQ(img->GetWidth(), 1);
		EXPECT_EQ(img->GetChannels(), 4);
		EXPECT_EQ(img->GetMips(), 1);
		EXPECT_EQ(img->GetImageAlignment(), ImageAlignment::COMPENSED);
		EXPECT_FALSE(img->GetRawPointer() == nullptr);
	}

	TEST(Utils, IsJPEGImageGeneratedUsingMemory)
	{
		uint32_t pixel = 0xFF0000FF;
		uint8_t* buffer;
		size_t bufferSize;
		Image::CastBMPToJPEG((unsigned char*)&pixel, 1, 1, &buffer, &bufferSize, false);
		std::shared_ptr<Image> img;
		img.reset(Image::CreateImage((std::byte*)buffer, bufferSize, ImageFormat::JPEG));
		EXPECT_EQ(img->GetHeight(), 1);
		EXPECT_EQ(img->GetWidth(), 1);
		EXPECT_EQ(img->GetChannels(), 4);
		EXPECT_EQ(img->GetMips(), 1);
		EXPECT_EQ(img->GetImageAlignment(), ImageAlignment::COMPENSED);
		EXPECT_FALSE(img->GetRawPointer() == nullptr);
	}

	TEST(Utils, IsPNGImageGeneratedUsingMemoryAndDimensions)
	{
		uint32_t pixel = 0xFF0000FF;
		uint8_t* buffer;
		size_t bufferSize;
		Image::CastBMPToPNG((unsigned char*)&pixel, 1, 1, &buffer, &bufferSize, false);
		std::shared_ptr<Image> img;
		img.reset(Image::CreateImage((std::byte*)buffer, 1, 1, ImageFormat::PNG));
		EXPECT_EQ(img->GetHeight(), 1);
		EXPECT_EQ(img->GetWidth(), 1);
		EXPECT_EQ(img->GetChannels(), 4);
		EXPECT_EQ(img->GetMips(), 1);
		EXPECT_EQ(img->GetImageAlignment(), ImageAlignment::COMPENSED);
		EXPECT_FALSE(img->GetRawPointer() == nullptr);
	}

	TEST(Utils, IsJPEGImageGeneratedUsingMemoryAndDimensions)
	{
		uint32_t pixel = 0xFF0000FF;
		uint8_t* buffer;
		size_t bufferSize;
		Image::CastBMPToJPEG((unsigned char*)&pixel, 1, 1, &buffer, &bufferSize, false);
		std::shared_ptr<Image> img;
		img.reset(Image::CreateImage((std::byte*)buffer, 1, 1, ImageFormat::JPEG));
		EXPECT_EQ(img->GetHeight(), 1);
		EXPECT_EQ(img->GetWidth(), 1);
		EXPECT_EQ(img->GetChannels(), 4);
		EXPECT_EQ(img->GetMips(), 1);
		EXPECT_EQ(img->GetImageAlignment(), ImageAlignment::COMPENSED);
		EXPECT_FALSE(img->GetRawPointer() == nullptr);
	}

	TEST(Utils, IsPNGMultiMips)
	{
		uint8_t pixels[12] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
		uint8_t* buffer;
		size_t bufferSize;
		Image::CastBMPToPNG((unsigned char*)&pixels[0], 2, 2, &buffer, &bufferSize, true, 3);
		std::shared_ptr<Image> img;
		img.reset(Image::CreateImage((std::byte*)buffer, bufferSize, ImageFormat::PNG));
		EXPECT_EQ(img->GetHeight(), 2);
		EXPECT_EQ(img->GetWidth(), 2);
		EXPECT_EQ(img->GetChannels(), 4);
		EXPECT_EQ(img->GetMips(), 2);
		EXPECT_EQ(img->GetImageAlignment(), ImageAlignment::COMPENSED);
		EXPECT_FALSE(img->GetRawPointer() == nullptr);
	}

	TEST(Utils, PNGGrayIsReading)
	{
		uint8_t pixels[4] = { 0xFF, 0xFF, 0xFF, 0xFF };
		uint8_t* buffer;
		size_t bufferSize;
		Image::CastBMPToPNG((unsigned char*)&pixels[0], 2, 2, &buffer, &bufferSize, true, 1);
		std::shared_ptr<Image> img;
		img.reset(Image::CreateImage((std::byte*)buffer, bufferSize, ImageFormat::PNG));
		EXPECT_FALSE(img->GetRawPointer() == nullptr);
	}

	TEST(Utils, ImageExtensionError)
	{
		uint32_t pixel = 0xFF0000FF;
		std::shared_ptr<Image> img;
		img.reset(Image::CreateImage("test"));
		EXPECT_TRUE(img.get() == nullptr);
	}

	TEST(Utils, ImageExtensionNotSupported)
	{
		uint32_t pixel = 0xFF0000FF;
		std::shared_ptr<Image> img;
		img.reset(Image::CreateImage("test.gif"));
		EXPECT_TRUE(img.get() == nullptr);
	}

	TEST(Utils, ImageFormatNotSupported)
	{
		uint8_t pixels[4] = { 0xFF, 0xFF, 0xFF, 0xFF };
		uint8_t* buffer;
		size_t bufferSize;
		Image::CastBMPToPNG((unsigned char*)&pixels[0], 2, 2, &buffer, &bufferSize, true, 1);
		std::shared_ptr<Image> img;
		img.reset(Image::CreateImage((std::byte*)buffer, bufferSize, ImageFormat::UNKNOWN));
		EXPECT_TRUE(img.get() == nullptr);
	}

	TEST(Utils, ImageFormatNotSupportedWithWidthAndHeight)
	{
		uint8_t pixels[4] = { 0xFF, 0xFF, 0xFF, 0xFF };
		uint8_t* buffer;
		size_t bufferSize;
		Image::CastBMPToPNG((unsigned char*)&pixels[0], 2, 2, &buffer, &bufferSize, true, 1);
		std::shared_ptr<Image> img;
		img.reset(Image::CreateImage((std::byte*)buffer, 2, 2, ImageFormat::UNKNOWN));
		EXPECT_TRUE(img.get() == nullptr);
	}
}