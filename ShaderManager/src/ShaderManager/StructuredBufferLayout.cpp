#include "StructuredBufferLayout.hpp"

Lust::StructuredBufferElement Lust::StructuredBufferLayout::s_EmptyElement = Lust::StructuredBufferElement();

Lust::StructuredBufferElement::StructuredBufferElement()
{
	m_Stride = 0;
	m_NumberOfBuffers = 0;
	m_Buffer = nullptr;
	m_AccessLevel = AccessLevel::ROOT_BUFFER;
	m_SpaceSet = 0;
	m_BindingSlot = 0;
	m_ShaderRegister = 0;
	m_BufferIndex = 0;
}

Lust::StructuredBufferElement::StructuredBufferElement(uint32_t bindingSlot, uint32_t shaderRegister, uint32_t spaceSet, uint32_t bufferIndex, size_t stride, size_t numberOfBuffers, AccessLevel accessLevel) :
	m_Buffer(nullptr), m_Stride(stride), m_NumberOfBuffers(numberOfBuffers), m_AccessLevel(accessLevel), m_SpaceSet(spaceSet), m_BindingSlot(bindingSlot), m_ShaderRegister(shaderRegister), m_BufferIndex(bufferIndex)
{
	m_Buffer = new uint8_t[stride * numberOfBuffers];
}

Lust::StructuredBufferElement::~StructuredBufferElement()
{
	delete[] m_Buffer;
}

const uint8_t* Lust::StructuredBufferElement::GetRawBuffer() const
{
	return m_Buffer;
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

size_t Lust::StructuredBufferElement::GetNumberOfBuffers() const
{
	return m_NumberOfBuffers;
}

size_t Lust::StructuredBufferElement::GetSize() const
{
	return m_Stride * m_NumberOfBuffers;
}

void Lust::StructuredBufferElement::CopyToBuffer(const void* buffer, size_t size, size_t offset) const
{
	if ((size + offset) <= (m_Stride * m_NumberOfBuffers))
	{
		memcpy(&m_Buffer[offset], buffer, size);
	}
}

Lust::StructuredBufferLayout::StructuredBufferLayout(std::initializer_list<StructuredBufferElement> elements, uint32_t allowedStages) :
	m_Stages(allowedStages)
{
	for (auto& element : elements)
	{
		uint64_t bufferLocation = ((uint64_t)element.GetShaderRegister() << 32) + element.GetBufferIndex();
		m_StructuredBuffers[bufferLocation] = element;
	}
}

const Lust::StructuredBufferElement& Lust::StructuredBufferLayout::GetElement(uint32_t shaderRegister, uint32_t textureIndex) const
{
	uint64_t bufferLocation = ((uint64_t)shaderRegister << 32) + textureIndex;
	auto it = m_StructuredBuffers.find(bufferLocation);
	if (it != m_StructuredBuffers.end())
		return it->second;
	else
		return s_EmptyElement;
}

const std::unordered_map<uint64_t, Lust::StructuredBufferElement>& Lust::StructuredBufferLayout::GetElements() const
{
	return m_StructuredBuffers;
}

uint32_t Lust::StructuredBufferLayout::GetStages() const
{
	return 0;
}
