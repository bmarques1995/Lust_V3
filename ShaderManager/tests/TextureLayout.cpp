#include <gtest/gtest.h>
#include "TextureLayout.hpp"

namespace Lust
{
	//uint32_t bindingSlot, uint32_t spaceSet, uint32_t shaderRegister, uint32_t textureIndex, std::string name
	TEST(TextureLayout, ValidateTextures)
	{
		TextureLayout layout(
			{
				{ 0, 0, 0, 0,"controller" },
				{ 1, 0, 1, 0, "controller2" }
			}, AllowedStages::PIXEL_STAGE);
		TextureElement e(2, 0, 2, 0, "controller3");
		layout.Upload(e);
		ASSERT_EQ(layout.GetElements().size(), 3);
		layout.Clear();
		ASSERT_EQ(layout.GetElements().size(), 0);
	}

	TEST(TextureLayout, ValidateValues)
	{
		TextureLayout layout(
			{
				{ 0, 0, 0, 0,"controller" },
				{ 2, 0, 2, 0, "controller2" }
			}, AllowedStages::PIXEL_STAGE);
		auto element = layout.GetElement("controller2");
		ASSERT_EQ(element.GetBindingSlot(), 2);
		ASSERT_EQ(element.GetSpaceSet(), 0);
		ASSERT_EQ(element.GetShaderRegister(), 2);
		ASSERT_EQ(element.GetTextureIndex(), 0);
		ASSERT_EQ(layout.GetStages(), AllowedStages::PIXEL_STAGE);
		ASSERT_STREQ(element.GetName().c_str(), "controller2");

		auto voidElement = layout.GetElement("controller3");
		ASSERT_STREQ(voidElement.GetName().c_str(), "");
	}

	//uint32_t bindingSlot, uint32_t spaceSet, uint32_t shaderRegister, uint32_t textureIndex, uint32_t numberOfTextures, std::string name
	TEST(TextureArrayLayout, ValidateTextures)
	{
		TextureArrayLayout layout(
			{
				{ 0, 0, 0, 0, 256,"controller" },
				{ 1, 0, 0, 256, 256, "controller2" }
			}, AllowedStages::PIXEL_STAGE);
		TextureArray e(2, 0, 0, 512, 256, "controller3");
		layout.Upload(e);
		ASSERT_EQ(layout.GetElements().size(), 3);
		layout.Clear();
		ASSERT_EQ(layout.GetElements().size(), 0);
	}

	TEST(TextureArrayLayout, ValidateValues)
	{
		TextureArrayLayout layout(
			{
				{ 0, 0, 0, 0, 256,"controller" },
				{ 1, 0, 0, 256, 256, "controller2" }
			}, AllowedStages::PIXEL_STAGE);
		auto element = layout.GetElement("controller2");
		ASSERT_EQ(element.GetBindingSlot(), 1);
		ASSERT_EQ(element.GetSpaceSet(), 0);
		ASSERT_EQ(element.GetShaderRegister(), 0);
		ASSERT_EQ(element.GetTextureIndex(), 256);
		ASSERT_EQ(element.GetNumberOfTextures(), 256);
		ASSERT_EQ(layout.GetStages(), AllowedStages::PIXEL_STAGE);
		ASSERT_STREQ(element.GetName().c_str(), "controller2");

		auto voidElement = layout.GetElement("controller3");
		ASSERT_STREQ(voidElement.GetName().c_str(), "");
	}

	TEST(TextureBuffer, EmptyBuffer)
	{
		std::shared_ptr<Image> image;
		TextureBuffer buffer(image, TextureTensor::TENSOR_2, "null_and_void");
		ASSERT_EQ(buffer.GetTensor(), TextureTensor::TENSOR_2);
		ASSERT_EQ(buffer.GetWidth(), 1);
		ASSERT_EQ(buffer.GetHeight(), 1);
		ASSERT_EQ(buffer.GetDepth(), 1);
		ASSERT_EQ(buffer.GetMipsLevel(), 1);
		ASSERT_EQ(buffer.GetChannels(), 4);
		ASSERT_NE(buffer.GetTextureBuffer(), nullptr);

		TextureBuffer buffer2;
		ASSERT_EQ(buffer2.GetMipsLevel(), 1);
		ASSERT_EQ(buffer2.GetAlignment(), ImageAlignment::COMPENSED);
		buffer.FreeImage();
		ASSERT_EQ(buffer.GetTextureBuffer(), nullptr);
		ASSERT_STREQ(buffer.GetFilepath().c_str(), "null_and_void");
	}

	TEST(TextureBuffer, CompleteBuffer)
	{
		std::shared_ptr<Image> image;
		uint32_t whitePixel = 0xffffffff;
		image.reset(Image::CreateImage((const std::byte*)&whitePixel, 1, 1, ImageFormat::PNG));
		TextureBuffer buffer(image, TextureTensor::TENSOR_2, "sample.png");
		ASSERT_STREQ(buffer.GetFilepath().c_str(), "sample.png");
	}
}