#include "StructuredBufferLayout.hpp"

Lust::StructuredBufferElement Lust::StructuredBufferLayout::s_EmptyElement = Lust::StructuredBufferElement();

Lust::StructuredBufferElement::StructuredBufferElement()
{
	m_Stride = 0;
	m_AccessLevel = AccessLevel::ROOT_BUFFER;
	m_SpaceSet = 0;
	m_BindingSlot = 0;
	m_ShaderRegister = 0;
	m_BufferIndex = 0;
	m_BufferAlignment = 0;
	m_NumberOfElements = 1;
	m_Name = "";
}

Lust::StructuredBufferElement::StructuredBufferElement(uint32_t bindingSlot, uint32_t shaderRegister, uint32_t spaceSet, uint32_t bufferIndex, size_t stride,
	AccessLevel accessLevel, size_t bufferAlignment, std::string name, uint32_t numberOfElements) :
	m_Stride(stride), m_AccessLevel(accessLevel), m_SpaceSet(spaceSet), m_BindingSlot(bindingSlot),
	m_ShaderRegister(shaderRegister), m_BufferIndex(bufferIndex), m_BufferAlignment(bufferAlignment), m_NumberOfElements(numberOfElements), m_Name(name)
{
	RecalculateBufferAlignment();
}

Lust::StructuredBufferElement::~StructuredBufferElement()
{
}

const std::string& Lust::StructuredBufferElement::GetName() const
{
	return m_Name;
}

Lust::BufferType Lust::StructuredBufferElement::GetBufferType() const
{
	return BufferType::STRUCTURED_BUFFER;
}

uint32_t Lust::StructuredBufferElement::GetBindingSlot() const
{
	return m_BindingSlot;
}

uint32_t Lust::StructuredBufferElement::GetShaderRegister() const
{
	return m_ShaderRegister;
}

uint32_t Lust::StructuredBufferElement::GetNumberOfElements() const
{
	return m_NumberOfElements;
}

void Lust::StructuredBufferElement::SetNumberOfElements(uint32_t numberOfElements) const
{
	m_NumberOfElements = numberOfElements;
	RecalculateBufferAlignment();
}

uint32_t Lust::StructuredBufferElement::GetSpaceSet() const
{
	return m_SpaceSet;
}

uint32_t Lust::StructuredBufferElement::GetBufferIndex() const
{
	return m_BufferIndex;
}

Lust::AccessLevel Lust::StructuredBufferElement::GetAccessLevel() const
{
	return m_AccessLevel;
}

size_t Lust::StructuredBufferElement::GetStride() const
{
	return m_Stride;
}

size_t Lust::StructuredBufferElement::GetSize() const
{
	return m_Stride * m_NumberOfElements;
}

size_t Lust::StructuredBufferElement::GetBufferAlignment() const
{
	return m_BufferAlignment;
}

size_t Lust::StructuredBufferElement::GetBufferCorrection() const
{
	return m_BufferCorrection;
}

void Lust::StructuredBufferElement::RecalculateBufferAlignment() const
{
	m_BufferCorrection = 0;
	if ((m_Stride * m_NumberOfElements) % m_BufferAlignment != 0)
		m_BufferCorrection = m_BufferAlignment - ((m_Stride * m_NumberOfElements) % m_BufferAlignment);
}

Lust::StructuredBufferLayout::StructuredBufferLayout(std::initializer_list<StructuredBufferElement> elements, uint32_t allowedStages) :
	m_Stages(allowedStages)
{
	for (auto& element : elements)
	{
		m_StructuredBuffers[element.GetName()] = element;
	}
}

const Lust::StructuredBufferElement& Lust::StructuredBufferLayout::GetElement(std::string elementName) const
{
	auto it = m_StructuredBuffers.find(elementName);
	if (it != m_StructuredBuffers.end())
		return it->second;
	else
		return s_EmptyElement;
}

const std::unordered_map<std::string, Lust::StructuredBufferElement>& Lust::StructuredBufferLayout::GetElements() const
{
	return m_StructuredBuffers;
}

Lust::StructuredBufferElement* Lust::StructuredBufferLayout::GetElementPointer(std::string elementName)
{
	auto it = m_StructuredBuffers.find(elementName);
	if (it != m_StructuredBuffers.end())
		return &(it->second);
	else
		return nullptr;
}

uint32_t Lust::StructuredBufferLayout::GetStages() const
{
	return m_Stages;
}

void Lust::StructuredBufferLayout::Clear()
{
	m_StructuredBuffers.clear();
}

void Lust::StructuredBufferLayout::Upload(const StructuredBufferElement& element)
{
	m_StructuredBuffers[element.GetName()] = element;
}
