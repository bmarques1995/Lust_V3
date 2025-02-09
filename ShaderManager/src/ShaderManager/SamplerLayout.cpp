#include "SamplerLayout.hpp"

Lust::SamplerElement Lust::SamplerLayout::s_EmptyElement = SamplerElement();

Lust::SamplerElement::SamplerElement()
{
	m_AddressMode = AddressMode::REPEAT;
	m_AnisotropicFactor = AnisotropicFactor::FACTOR_0;
	m_ComparisonPassMode = ComparisonPassMode::NEVER;
	m_Filter = SamplerFilter::LINEAR;
	m_SpaceSet = 0;
	m_ShaderRegister = 0;
	m_BindingSlot = 0;
	m_SamplerIndex = 0;
}

Lust::SamplerElement::SamplerElement(SamplerFilter filter, AnisotropicFactor anisotropicFactor, AddressMode addressMode, ComparisonPassMode comparisonPassMode, uint32_t bindingSlot, uint32_t spaceSet, uint32_t shaderRegister, uint32_t samplerIndex) :
	m_Filter(filter),
	m_AnisotropicFactor(anisotropicFactor),
	m_AddressMode(addressMode),
	m_ComparisonPassMode(comparisonPassMode),
	m_BindingSlot(bindingSlot),
	m_SpaceSet(spaceSet),
	m_ShaderRegister(shaderRegister),
	m_SamplerIndex(samplerIndex)
{
}

Lust::SamplerFilter Lust::SamplerElement::GetFilter() const
{
	return m_Filter;
}

Lust::AnisotropicFactor Lust::SamplerElement::GetAnisotropicFactor() const
{
	return m_AnisotropicFactor;
}

Lust::AddressMode Lust::SamplerElement::GetAddressMode() const
{
	return m_AddressMode;
}

Lust::ComparisonPassMode Lust::SamplerElement::GetComparisonPassMode() const
{
	return m_ComparisonPassMode;
}

uint32_t Lust::SamplerElement::GetBindingSlot() const
{
	return m_BindingSlot;
}

uint32_t Lust::SamplerElement::GetSpaceSet() const
{
	return m_SpaceSet;
}

uint32_t Lust::SamplerElement::GetShaderRegister() const
{
	return m_ShaderRegister;
}

uint32_t Lust::SamplerElement::GetSamplerIndex() const
{
	return m_SamplerIndex;
}

Lust::SamplerLayout::SamplerLayout(std::initializer_list<SamplerElement> elements)
{
	for (auto& element : elements)
	{
		uint64_t samplerLocation = ((uint64_t)element.GetShaderRegister() << 32) + element.GetSamplerIndex();
		m_Samplers[samplerLocation] = element;
	}
}

const Lust::SamplerElement& Lust::SamplerLayout::GetElement(uint32_t shaderRegister, uint32_t samplerIndex) const
{
	uint64_t samplerLocation = ((uint64_t)shaderRegister << 32) + samplerIndex;
	auto it = m_Samplers.find(samplerLocation);
	if (it != m_Samplers.end())
		return it->second;
	else
		return s_EmptyElement;
}

const std::unordered_map<uint64_t, Lust::SamplerElement>& Lust::SamplerLayout::GetElements() const
{
	return m_Samplers;
}