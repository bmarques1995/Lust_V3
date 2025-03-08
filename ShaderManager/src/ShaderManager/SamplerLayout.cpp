#include "SamplerLayout.hpp"

Lust::SamplerElement Lust::SamplerLayout::s_EmptyElement = SamplerElement();

Lust::SamplerElement::SamplerElement()
{
	m_SpaceSet = 0;
	m_ShaderRegister = 0;
	m_BindingSlot = 0;
	m_SamplerIndex = 0;
	m_Name = "";
}

Lust::SamplerElement::SamplerElement(uint32_t bindingSlot, uint32_t spaceSet, uint32_t shaderRegister, uint32_t samplerIndex, const std::string& name) :
	m_BindingSlot(bindingSlot),
	m_SpaceSet(spaceSet),
	m_ShaderRegister(shaderRegister),
	m_SamplerIndex(samplerIndex),
	m_Name(name)
{
}

const std::string& Lust::SamplerElement::GetName() const
{
	return m_Name;
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
		m_Samplers[element.GetName()] = element;
	}
}

const Lust::SamplerElement& Lust::SamplerLayout::GetElement(std::string name) const
{
	auto it = m_Samplers.find(name);
	if (it != m_Samplers.end())
		return it->second;
	else
		return s_EmptyElement;
}

const std::unordered_map<std::string, Lust::SamplerElement>& Lust::SamplerLayout::GetElements() const
{
	return m_Samplers;
}

void Lust::SamplerLayout::Clear()
{
	m_Samplers.clear();
}

void Lust::SamplerLayout::Upload(const SamplerElement& element)
{
	m_Samplers[element.GetName()] = element;
}

Lust::SamplerInfo::SamplerInfo()
{
	m_Filter = SamplerFilter::ANISOTROPIC;
	m_AnisotropicFactor = AnisotropicFactor::FACTOR_3;
	m_AddressMode = AddressMode::BORDER;
	m_ComparisonPassMode = ComparisonPassMode::ALWAYS;
}
Lust::SamplerInfo::SamplerInfo(SamplerFilter filter, AnisotropicFactor anisotropicFactor, AddressMode addressMode, ComparisonPassMode comparisonPassMode) :
	m_Filter(filter),
	m_AnisotropicFactor(anisotropicFactor),
	m_AddressMode(addressMode),
	m_ComparisonPassMode(comparisonPassMode)
{
}

Lust::SamplerFilter Lust::SamplerInfo::GetFilter() const
{
	return m_Filter;
}

Lust::AnisotropicFactor Lust::SamplerInfo::GetAnisotropicFactor() const
{
	return m_AnisotropicFactor;
}

Lust::AddressMode Lust::SamplerInfo::GetAddressMode() const
{
	return m_AddressMode;
}

Lust::ComparisonPassMode Lust::SamplerInfo::GetComparisonPassMode() const
{
	return m_ComparisonPassMode;
}
