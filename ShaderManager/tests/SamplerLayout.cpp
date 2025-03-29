#include <gtest/gtest.h>
#include "SamplerLayout.hpp"

namespace Lust
{
	//uint32_t bindingSlot, uint32_t spaceSet, uint32_t shaderRegister, uint32_t textureIndex, std::string name
	TEST(SamplerLayout, ValidateSamplers)
	{
		SamplerLayout layout(
			{
				{ 0, 0, 0, 0,"controller" },
				{ 1, 0, 1, 0, "controller2" }
			});
		SamplerElement e(2, 0, 2, 0, "controller3");
		layout.Upload(e);
		ASSERT_EQ(layout.GetElements().size(), 3);
		layout.Clear();
		ASSERT_EQ(layout.GetElements().size(), 0);
	}

	TEST(SamplerLayout, ValidateValues)
	{
		//uint32_t bindingSlot, uint32_t spaceSet, uint32_t shaderRegister, uint32_t samplerIndex, const std::string& name
		SamplerLayout layout(
			{
				{ 0, 0, 0, 0, "controller" },
				{ 2, 0, 2, 0, "controller2" }
			});
		auto element = layout.GetElement("controller2");
		ASSERT_EQ(element.GetBindingSlot(), 2);
		ASSERT_EQ(element.GetSpaceSet(), 0);
		ASSERT_EQ(element.GetShaderRegister(), 2);
		ASSERT_EQ(element.GetSamplerIndex(), 0);
		ASSERT_STREQ(element.GetName().c_str(), "controller2");

		auto voidElement = layout.GetElement("controller3");
		ASSERT_STREQ(voidElement.GetName().c_str(), "");
	}

	//uint32_t bindingSlot, uint32_t spaceSet, uint32_t shaderRegister, uint32_t textureIndex, uint32_t numberOfTextures, std::string name
	TEST(SamplerArrayLayout, ValidateSamplers)
	{
		SamplerArrayLayout layout(
			{
				{ 0, 0, 0, 0, 256,"controller" },
				{ 1, 0, 0, 256, 256, "controller2" }
			});
		SamplerArray e(2, 0, 0, 512, 256, "controller3");
		layout.Upload(e);
		ASSERT_EQ(layout.GetElements().size(), 3);
		layout.Clear();
		ASSERT_EQ(layout.GetElements().size(), 0);
	}

	TEST(SamplerArrayLayout, ValidateValues)
	{
		SamplerArrayLayout layout(
			{
				{ 0, 0, 0, 0, 256,"controller" },
				{ 1, 0, 0, 256, 256, "controller2" }
			});
		auto element = layout.GetElement("controller2");
		ASSERT_EQ(element.GetBindingSlot(), 1);
		ASSERT_EQ(element.GetSpaceSet(), 0);
		ASSERT_EQ(element.GetShaderRegister(), 0);
		ASSERT_EQ(element.GetSamplerIndex(), 256);
		ASSERT_EQ(element.GetNumberOfSamplers(), 256);
		ASSERT_STREQ(element.GetName().c_str(), "controller2");

		auto voidElement = layout.GetElement("controller3");
		ASSERT_STREQ(voidElement.GetName().c_str(), "");
	}

	TEST(SamplerInfo, EmptyBuffer)
	{
		//SamplerFilter filter, AnisotropicFactor anisotropicFactor, AddressMode addressMode, ComparisonPassMode comparisonPassMode
		SamplerInfo sampler;
		ASSERT_EQ(sampler.GetFilter(), SamplerFilter::ANISOTROPIC);
		ASSERT_EQ(sampler.GetAnisotropicFactor(), AnisotropicFactor::FACTOR_3);
		ASSERT_EQ(sampler.GetAddressMode(), AddressMode::BORDER);
		ASSERT_EQ(sampler.GetComparisonPassMode(), ComparisonPassMode::ALWAYS);
	}

	TEST(SamplerInfo, CompleteBuffer)
	{
		SamplerInfo sampler(SamplerFilter::LINEAR, AnisotropicFactor::FACTOR_4, AddressMode::REPEAT, ComparisonPassMode::ALWAYS);
		ASSERT_EQ(sampler.GetFilter(), SamplerFilter::LINEAR);
		ASSERT_EQ(sampler.GetAnisotropicFactor(), AnisotropicFactor::FACTOR_4);
		ASSERT_EQ(sampler.GetAddressMode(), AddressMode::REPEAT);
		ASSERT_EQ(sampler.GetComparisonPassMode(), ComparisonPassMode::ALWAYS);
	}
}