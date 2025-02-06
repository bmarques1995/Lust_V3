#include "UniformsLayout.hpp"
#include <sstream>

Lust::AttachmentMismatchException::AttachmentMismatchException(size_t bufferSize, size_t expectedBufferAttachment)
{
	std::stringstream buffer;
	buffer << "Is expected the buffer be multiple of " << expectedBufferAttachment <<
		", but the module of the division between the buffer size and the expected attachment is " << (bufferSize % expectedBufferAttachment);
	m_Reason = buffer.str();
}

Lust::SignatureMismatchException::SignatureMismatchException(uint32_t numberOfBuffers)
{
	std::stringstream buffer;
	buffer << "Root CBVs can handle only 1 CBV per time but are handling " << numberOfBuffers << " CBVs";
	m_Reason = buffer.str();
}

Lust::SmallBufferElement::SmallBufferElement()
{
	m_Offset = 0;
	m_Size = 0;
	m_BindingSlot = 0xffff;
}

Lust::SmallBufferElement::SmallBufferElement(size_t offset, size_t size, uint32_t bindingSlot, uint32_t smallAttachment) :
	m_Offset(offset), m_Size(size), m_BindingSlot(bindingSlot)
{
	if (!IsSizeValid(smallAttachment))
		throw AttachmentMismatchException(size, smallAttachment);
}

size_t Lust::SmallBufferElement::GetOffset() const
{
	return m_Offset;
}

size_t Lust::SmallBufferElement::GetSize() const
{
	return m_Size;
}

uint32_t Lust::SmallBufferElement::GetBindingSlot() const
{
	return m_BindingSlot;
}

bool Lust::SmallBufferElement::IsSizeValid(uint32_t smallAttachment)
{
	return ((m_Size % smallAttachment) == 0);
}

Lust::SmallBufferLayout::SmallBufferLayout(std::initializer_list<SmallBufferElement> m_Elements, uint32_t stages) :
	m_Stages(stages)
{
	for (auto& element : m_Elements)
	{
		m_Buffers[element.GetBindingSlot()] = element;
	}
}

const Lust::SmallBufferElement& Lust::SmallBufferLayout::GetElement(uint32_t bindingSlot)
{
	return m_Buffers[bindingSlot];
}

const std::unordered_map<uint32_t, Lust::SmallBufferElement>& Lust::SmallBufferLayout::GetElements()
{
	return m_Buffers;
}

uint32_t Lust::SmallBufferLayout::GetStages() const
{
	return m_Stages;
}

Lust::UniformElement::UniformElement()
{
	m_BufferType = BufferType::INVALID_BUFFER_TYPE;
	m_Size = 0;
	m_BindingSlot = 0xffff;
	m_ShaderRegister = 0;
	m_SpaceSet = 0;
	m_AccessLevel = AccessLevel::ROOT_BUFFER;
	m_NumberOfBuffers = 1;
}


//BufferType bufferType, size_t size, uint32_t bindingSlot, uint32_t shaderRegister, uint32_t spaceSet, uint32_t bufferAttachment
Lust::UniformElement::UniformElement(BufferType bufferType, size_t size, uint32_t bindingSlot, uint32_t spaceSet, uint32_t shaderRegister, AccessLevel accessLevel, uint32_t numberOfBuffers, uint32_t bufferAttachment) :
	m_BufferType(bufferType), m_Size(size), m_BindingSlot(bindingSlot), m_SpaceSet(spaceSet), m_ShaderRegister(shaderRegister), m_AccessLevel(accessLevel), m_NumberOfBuffers(numberOfBuffers)
{
	if (!IsSizeValid(bufferAttachment))
		throw AttachmentMismatchException(size, bufferAttachment);
	if ((m_AccessLevel == AccessLevel::ROOT_BUFFER) && (m_NumberOfBuffers != 1))
		throw SignatureMismatchException(m_NumberOfBuffers);
}

Lust::BufferType Lust::UniformElement::GetBufferType() const
{
	return m_BufferType;
}

size_t Lust::UniformElement::GetSize() const
{
	return m_Size;
}

uint32_t Lust::UniformElement::GetBindingSlot() const
{
	return m_BindingSlot;
}

uint32_t Lust::UniformElement::GetSpaceSet() const
{
	return m_SpaceSet;
}

uint32_t Lust::UniformElement::GetShaderRegister() const
{
	return m_ShaderRegister;
}

Lust::AccessLevel Lust::UniformElement::GetAccessLevel() const
{
	return m_AccessLevel;
}

uint32_t Lust::UniformElement::GetNumberOfBuffers() const
{
	return m_NumberOfBuffers;
}

bool Lust::UniformElement::IsSizeValid(uint32_t bufferAttachment)
{
	return ((m_Size % bufferAttachment) == 0);
}

Lust::UniformLayout::UniformLayout(std::initializer_list<UniformElement> m_Elements, uint32_t allowedStages) :
	m_Stages(allowedStages)
{
	for (auto& element : m_Elements)
	{
		m_Buffers[element.GetShaderRegister()] = element;
	}
}

const Lust::UniformElement& Lust::UniformLayout::GetElement(uint32_t shaderRegister)
{
	return m_Buffers[shaderRegister];
}

const std::unordered_map<uint32_t, Lust::UniformElement>& Lust::UniformLayout::GetElements()
{
	return m_Buffers;
}

uint32_t Lust::UniformLayout::GetStages() const
{
	return m_Stages;
}
