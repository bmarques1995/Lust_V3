#include <gtest/gtest.h>
#include "StructuredBufferLayout.hpp"

namespace Lust
{
	//uint32_t bindingSlot, uint32_t shaderRegister, uint32_t spaceSet, uint32_t bufferIndex, size_t stride, AccessLevel accessLevel, size_t bufferAlignment, std::string name, uint32_t numberOfElements = 1
	TEST(StructuredBufferLayout, ValidateStructuredBuffer)
	{
		StructuredBufferLayout layout(
			{
				{ 0, 0, 0, 0, 80, AccessLevel::ROOT_BUFFER, 256, "controller", 100 },
				{ 0, 1, 1, 0, 64, AccessLevel::ROOT_BUFFER, 256, "controller2", 100 }
			}, AllowedStages::VERTEX_STAGE);
		StructuredBufferElement e(0, 1, 1, 0, 256, AccessLevel::ROOT_BUFFER, 256, "controller3", 100);
		layout.Upload(e);
		ASSERT_EQ(layout.GetElements().size(), 3);
		auto element = layout.GetElement("controller");
		ASSERT_EQ(element.GetSize() + element.GetBufferCorrection(), 8192);
		ASSERT_EQ((element.GetSize() + element.GetBufferCorrection()) % element.GetBufferAlignment(), 0);
		layout.Clear();
		ASSERT_EQ(layout.GetElements().size(), 0);
	}

	TEST(StructuredBufferLayout, ValidateValues)
	{

		StructuredBufferLayout layout(
			{
				{ 0, 0, 0, 0, 1280, AccessLevel::ROOT_BUFFER, 256, "controller", 100 },
			}, AllowedStages::VERTEX_STAGE);

		auto element = layout.GetElement("controller");
		auto element2 = layout.GetElement("controller4");
		auto pElement = layout.GetElementPointer("controller");
		auto pElement2 = layout.GetElementPointer("controller4");
		ASSERT_EQ(element.GetBufferType(), BufferType::STRUCTURED_BUFFER);
		ASSERT_EQ(element.GetBindingSlot(), 0);
		ASSERT_EQ(element.GetSpaceSet(), 0);
		ASSERT_EQ(element.GetShaderRegister(), 0);
		ASSERT_EQ(element.GetSize(), 128000);
		ASSERT_EQ(element.GetStride(), 1280);
		ASSERT_EQ(element.GetNumberOfElements(), 100);
		ASSERT_EQ(element.GetBufferIndex(), 0);
		pElement->SetNumberOfElements(400);
		ASSERT_EQ(pElement->GetSize(), 512000);
		ASSERT_EQ(element.GetAccessLevel(), AccessLevel::ROOT_BUFFER);
		ASSERT_EQ(layout.GetStages(), AllowedStages::VERTEX_STAGE);
		ASSERT_EQ(pElement2, nullptr);
		ASSERT_STREQ(element.GetName().c_str(), "controller");

		ASSERT_STREQ(element2.GetName().c_str(), "");
	}
}