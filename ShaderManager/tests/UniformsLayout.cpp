#include <gtest/gtest.h>
#include "UniformsLayout.hpp"

namespace Lust
{
	//BufferType bufferType, size_t size, uint32_t bindingSlot, uint32_t spaceSet, uint32_t shaderRegister, AccessLevel accessLevel, uint32_t numberOfBuffers, uint32_t bufferAttachment, uint32_t tableIndex, std::string name
	TEST(UniformsLayout, ValidateUniformsLayout)
	{
		UniformLayout layout(
		{
			{ BufferType::UNIFORM_CONSTANT_BUFFER, 256, 0, 0, 0, AccessLevel::ROOT_BUFFER, 1, 256, 0, "controller"  },
			{ BufferType::UNIFORM_CONSTANT_BUFFER, 256, 1, 0, 1, AccessLevel::ROOT_BUFFER, 1, 256, 0, "controller2" }
		}, AllowedStages::VERTEX_STAGE);
		UniformElement e(BufferType::UNIFORM_CONSTANT_BUFFER, 512, 0, 0, 0, AccessLevel::ROOT_BUFFER, 1, 256, 0, "controller3");
		layout.Upload(e);
		ASSERT_EQ(layout.GetElements().size(), 3);
		layout.Clear();
		ASSERT_EQ(layout.GetElements().size(), 0);
	}

	TEST(UniformsLayout, ValidateSizeAndElementsException)
	{

		ASSERT_THROW(UniformElement e(BufferType::UNIFORM_CONSTANT_BUFFER, 384, 0, 0, 0, AccessLevel::ROOT_BUFFER, 1, 256, 0, "controller"), AttachmentMismatchException);
		ASSERT_THROW(UniformElement e(BufferType::UNIFORM_CONSTANT_BUFFER, 512, 0, 0, 0, AccessLevel::ROOT_BUFFER, 3, 256, 0, "controller"), SignatureMismatchException);
	}

	TEST(UniformsLayout, ValidateValues)
	{

		UniformLayout layout(
			{
				{ BufferType::UNIFORM_CONSTANT_BUFFER, 256, 0, 0, 0, AccessLevel::ROOT_BUFFER, 1, 256, 0, "controller"  },
				{ BufferType::UNIFORM_CONSTANT_BUFFER, 256, 1, 0, 1, AccessLevel::ROOT_BUFFER, 1, 256, 0, "controller2" }
			}, AllowedStages::VERTEX_STAGE);

		auto element = layout.GetElement("controller");
		auto element2 = layout.GetElement("controller4");
		ASSERT_EQ(element.GetBufferType(), BufferType::UNIFORM_CONSTANT_BUFFER);
		ASSERT_EQ(element.GetBindingSlot(), 0);
		ASSERT_EQ(element.GetSpaceSet(), 0);
		ASSERT_EQ(element.GetShaderRegister(), 0);
		ASSERT_EQ(element.GetSize(), 256);
		ASSERT_EQ(element.GetAccessLevel(), AccessLevel::ROOT_BUFFER);
		ASSERT_EQ(element.GetNumberOfBuffers(), 1);
		ASSERT_EQ(element.GetTableIndex(), 0);
		ASSERT_EQ(layout.GetStages(), AllowedStages::VERTEX_STAGE);
		ASSERT_STREQ(element.GetName().c_str(), "controller");

		ASSERT_STREQ(element2.GetName().c_str(), "");
	}

	TEST(SmallBufferLayout, ValidateSmallBufferLayout)
	{
		//size_t offset, size_t size, uint32_t bindingSlot, uint32_t smallAttachment, std::string name
		SmallBufferLayout layout(
			{
				{ 0, 64, 0, 4, "controller"  }
			}, AllowedStages::VERTEX_STAGE);
		SmallBufferElement e(0, 64, 1, 4, "controller2");
		layout.Upload(e);
		ASSERT_EQ(layout.GetElements().size(), 2);
		layout.Clear();
		ASSERT_EQ(layout.GetElements().size(), 0);
	}

	TEST(SmallBufferLayout, ValidateSizeAndElementsException)
	{

		ASSERT_THROW(SmallBufferElement element( 0, 74, 0, 4, "controller"  ), AttachmentMismatchException);
	}

	TEST(SmallBufferLayout, ValidateValues)
	{

		SmallBufferLayout layout(
			{
				{ 0, 64, 0, 4, "controller"  }
			}, AllowedStages::VERTEX_STAGE);

		auto element = layout.GetElement("controller");
		auto element2 = layout.GetElement("controller4");
		ASSERT_EQ(element.GetOffset(), 0);
		ASSERT_EQ(element.GetBindingSlot(), 0);
		ASSERT_EQ(element.GetSize(), 64);
		ASSERT_EQ(layout.GetStages(), AllowedStages::VERTEX_STAGE);
		ASSERT_STREQ(element.GetName().c_str(), "controller");

		ASSERT_STREQ(element2.GetName().c_str(), "");
	}
}
