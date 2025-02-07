#pragma once

#include "LustShaderManagerDLLMacro.hpp"
#include <cstdint>
#include <unordered_map>
#include <initializer_list>

namespace Lust
{
	enum class LUST_SHADER_MNG_API SamplerFilter
	{
		LINEAR,
		NEAREST,
		ANISOTROPIC
	};

	enum class LUST_SHADER_MNG_API AnisotropicFactor
	{
		FACTOR_0,
		FACTOR_1,
		FACTOR_2,
		FACTOR_3,
		FACTOR_4,
	};

	enum class LUST_SHADER_MNG_API AddressMode
	{
		REPEAT,
		MIRROR,
		CLAMP,
		BORDER,
		MIRROR_ONCE
	};

	enum class LUST_SHADER_MNG_API ComparisonPassMode
	{
		NEVER,
		LESS,
		EQUAL,
		LESS_EQUAL,
		GREATER,
		NEQ,
		GREATER_EQUAL,
		ALWAYS
	};

	class LUST_SHADER_MNG_API SamplerElement
	{
	public:
		SamplerElement();
		SamplerElement(SamplerFilter filter, AnisotropicFactor anisotropicFactor, AddressMode addressMode, ComparisonPassMode comparisonPassMode, uint32_t bindingSlot, uint32_t spaceSet, uint32_t shaderRegister, uint32_t samplerIndex);

		SamplerFilter GetFilter() const;
		AnisotropicFactor GetAnisotropicFactor() const;
		AddressMode GetAddressMode() const;
		ComparisonPassMode GetComparisonPassMode() const;
		uint32_t GetBindingSlot() const;
		uint32_t GetSpaceSet() const;
		uint32_t GetShaderRegister() const;
		uint32_t GetSamplerIndex() const;

	private:
		SamplerFilter m_Filter;
		AnisotropicFactor m_AnisotropicFactor;
		AddressMode m_AddressMode;
		ComparisonPassMode m_ComparisonPassMode;
		uint32_t m_BindingSlot;
		uint32_t m_SpaceSet;
		uint32_t m_ShaderRegister;
		uint32_t m_SamplerIndex;
	};

	class LUST_SHADER_MNG_API SamplerLayout
	{
	public:
		SamplerLayout(std::initializer_list<SamplerElement> elements);

		const SamplerElement& GetElement(uint32_t shaderRegister, uint32_t samplerIndex);
		const std::unordered_map<uint64_t, SamplerElement>& GetElements();

	private:
		std::unordered_map<uint64_t, SamplerElement> m_Samplers;
	};
}
