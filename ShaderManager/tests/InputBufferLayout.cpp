#include <gtest/gtest.h>
#include "InputBufferLayout.hpp"
#include "Console.hpp"

namespace Lust
{
	TEST(InputBufferLayout, ValidateStrideAndOffset)
	{
		InputBufferElement element(ShaderDataType::Float3, "position", false);
		InputBufferElement element2(ShaderDataType::Float2, "txc", false);
		InputBufferLayout layout;
		layout.PushBack(element);
		layout.PushBack(element2);
		ASSERT_EQ(layout.GetStride(), sizeof(float) * 5);
		ASSERT_EQ(element.GetComponentCount(), 3);
		ASSERT_EQ(element2.GetComponentCount(), 2);
		auto elements = layout.GetElements();
		size_t offset = 0;
		for (auto& element : elements)
		{
			ASSERT_EQ(element.GetOffset(), offset);
			offset += element.GetSize();
		}
		layout.Clear();
		ASSERT_EQ(layout.GetStride(), 0);
	}

	TEST(InputBufferLayout, ValidateNameTypeAndNormalization)
	{
		Console::Init();
		InputBufferElement element(ShaderDataType::Float, "position", false);
		InputBufferElement element2(ShaderDataType::Float4, "txc", false);
		InputBufferLayout layout(
		{
			element,
			element2
		});
		ASSERT_EQ(layout.GetStride(), sizeof(float) * 5);
		ASSERT_FALSE(element.IsNormalized());
		ASSERT_STREQ(element.GetName().c_str(), "position");
		ASSERT_EQ(element.GetType(), ShaderDataType::Float);
		ASSERT_EQ(element.GetComponentCount(), 1);
		ASSERT_EQ(element2.GetComponentCount(), 4);
		InputBufferElement element3;
		ASSERT_EQ(element3.GetType(), ShaderDataType::None);
		ASSERT_EQ(ShaderDataTypeSize(element3.GetType()), 0);
		ASSERT_FALSE(element3.IsNormalized());
		ASSERT_EQ(element3.GetComponentCount(), 0);
		Console::End();
	}

	TEST(InputBufferLayout, ValidateIterators)
	{
		Console::Init();
		InputBufferElement element(ShaderDataType::Float, "position", false);
		InputBufferElement element2(ShaderDataType::Float4, "txc", false);
		InputBufferLayout layout(
			{
				element,
				element2
			});
		auto it = layout.begin();
		auto it2 = layout.end();
		ASSERT_EQ(it, layout.begin());
		ASSERT_EQ(it2, layout.end());

		std::vector<InputBufferElement>::const_iterator it3 = layout.const_begin();
		std::vector<InputBufferElement>::const_iterator it4 = layout.const_end();

		ASSERT_EQ(it3, layout.const_begin());
		ASSERT_EQ(it4, layout.const_end());
		Console::End();
	}
}