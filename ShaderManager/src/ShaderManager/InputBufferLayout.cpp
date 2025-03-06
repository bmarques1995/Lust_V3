#include "InputBufferLayout.hpp"

uint32_t Lust::ShaderDataTypeSize(ShaderDataType type)
{
	switch (type)
	{
	case ShaderDataType::Float:    return 4;
	case ShaderDataType::Float2:   return 4 * 2;
	case ShaderDataType::Float3:   return 4 * 3;
	case ShaderDataType::Float4:   return 4 * 4;
	case ShaderDataType::Uint:      return 4;
	case ShaderDataType::Uint2:     return 4 * 2;
	case ShaderDataType::Uint3:     return 4 * 3;
	case ShaderDataType::Uint4:     return 4 * 4;
	}

	Console::CoreError("Unknown ShaderDataType!");
	assert(false);
	return 0;
}

Lust::InputBufferElement::InputBufferElement()
{
	m_Name = "";
	m_Type = ShaderDataType::None;
	m_Size = 0;
	m_Offset = 0;
	m_Normalized = false;
}

Lust::InputBufferElement::InputBufferElement(ShaderDataType type, const std::string& name, bool normalized) :
	m_Name(name), m_Type(type), m_Size(ShaderDataTypeSize(type)), m_Offset(0), m_Normalized(normalized)
{
}

uint32_t Lust::InputBufferElement::GetComponentCount() const
{
	switch (m_Type)
	{
	case ShaderDataType::Float:   return 1;
	case ShaderDataType::Float2:  return 2;
	case ShaderDataType::Float3:  return 3;
	case ShaderDataType::Float4:  return 4;
	case ShaderDataType::Uint:     return 1;
	case ShaderDataType::Uint2:    return 2;
	case ShaderDataType::Uint3:    return 3;
	case ShaderDataType::Uint4:    return 4;
	}

	Console::CoreAssert(false, "Unknown ShaderDataType!");
	return 0;
}

const std::string& Lust::InputBufferElement::GetName() const
{
	return m_Name;
}

const Lust::ShaderDataType Lust::InputBufferElement::GetType() const
{
	return m_Type;
}

const uint32_t Lust::InputBufferElement::GetSize() const
{
	return m_Size;
}

const uint32_t Lust::InputBufferElement::GetOffset() const
{
	return m_Offset;
}

const bool Lust::InputBufferElement::IsNormalized() const
{
	return m_Normalized;
}

Lust::InputBufferLayout::InputBufferLayout(const std::initializer_list<InputBufferElement>& elements) :
	m_Elements(elements)
{
	CalculateOffsetsAndStride();
}

Lust::InputBufferLayout::InputBufferLayout(const std::vector<InputBufferElement>& elements) :
	m_Elements(elements)
{
	CalculateOffsetsAndStride();
}

void Lust::InputBufferLayout::Clear()
{
	m_Elements.clear();
	m_Stride = 0;
}

void Lust::InputBufferLayout::PushBack(const InputBufferElement& element)
{
	m_Elements.push_back(element);
	CalculateOffsetsAndStride();
}

void Lust::InputBufferLayout::CalculateOffsetsAndStride()
{
	uint32_t offset = 0;
	m_Stride = 0;
	for (auto& element : m_Elements)
	{
		element.m_Offset = offset;
		offset += element.m_Size;
		m_Stride += element.m_Size;
	}
}
