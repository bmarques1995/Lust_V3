#include "TextureLayout.hpp"
#include <algorithm>

Lust::TextureElement Lust::TextureLayout::s_EmptyElement = TextureElement();

Lust::TextureElement::TextureElement()
{
	m_BindingSlot = 0xffff;
	m_ShaderRegister = 0;
	m_SpaceSet = 0;
	m_TextureIndex = 0;
	m_Name = "";
}

//std::shared_ptr<Image> img, uint32_t bindingSlot, uint32_t shaderRegister, uint32_t spaceSet, uint32_t samplerRegister, TextureTensor tensor, size_t depth = 1
Lust::TextureElement::TextureElement(uint32_t bindingSlot, uint32_t shaderRegister, uint32_t spaceSet, uint32_t textureIndex, std::string name) :
	m_BindingSlot(bindingSlot), m_ShaderRegister(shaderRegister), m_SpaceSet(spaceSet), m_TextureIndex(textureIndex), m_Name(name)
{
}

const std::string& Lust::TextureElement::GetName() const
{
	return m_Name;
}

uint32_t Lust::TextureElement::GetBindingSlot() const
{
	return m_BindingSlot;
}

uint32_t Lust::TextureElement::GetShaderRegister() const
{
	return m_ShaderRegister;
}

uint32_t Lust::TextureElement::GetSpaceSet() const
{
	return m_SpaceSet;
}


uint32_t Lust::TextureElement::GetTextureIndex() const
{
	return m_TextureIndex;
}

Lust::TextureLayout::TextureLayout(std::initializer_list<TextureElement> elements, uint32_t allowedStages) :
	m_Stages(allowedStages)
{
	for (auto& element : elements)
	{
		m_Textures[element.GetName()] = element;
	}
}

const Lust::TextureElement& Lust::TextureLayout::GetElement(std::string elementName) const
{
	auto it = m_Textures.find(elementName);
	if (it != m_Textures.end())
		return it->second;
	else
		return s_EmptyElement;
}

const std::unordered_map<std::string, Lust::TextureElement>& Lust::TextureLayout::GetElements() const
{
	return m_Textures;
}

uint32_t Lust::TextureLayout::GetStages() const
{
	return m_Stages;
}

Lust::TextureBuffer::TextureBuffer()
{
	uint32_t whitePixel = 0xffffffff;
	m_Depth = 1;
	m_Tensor = TextureTensor::TENSOR_2;
	m_Image.reset(Image::CreateImage((const std::byte*)&whitePixel, 1, 1, ImageFormat::PNG));
	m_Width = 1;
	m_Height = 1;
	m_MipsLevel = 1;
	m_Channels = 4;
	m_Filepath = "";
}

Lust::TextureBuffer::TextureBuffer(std::shared_ptr<Image> img, TextureTensor tensor, std::string filepath, size_t depth) :
	m_Image(img), m_Tensor(tensor), m_Filepath(filepath)
{
	m_Depth = std::max<size_t>(1, depth);
	if (img)
	{
		m_Width = img->GetWidth();
		m_Height = img->GetHeight();
		m_MipsLevel = img->GetMips();
		m_Channels = img->GetChannels();
	}
	else
	{
		m_Width = 0;
		m_Height = 0;
		m_MipsLevel = 0;
		m_Channels = 4;
	}
}

Lust::TextureBuffer::~TextureBuffer()
{

}

const uint8_t* Lust::TextureBuffer::GetTextureBuffer() const
{
	if (m_Image)
		return m_Image->GetRawPointer();
	else
		return nullptr;
}

uint32_t Lust::TextureBuffer::GetWidth() const
{
	return m_Width;
}

uint32_t Lust::TextureBuffer::GetHeight() const
{
	return m_Height;
}

uint32_t Lust::TextureBuffer::GetDepth() const
{
	return m_Depth;
}

uint32_t Lust::TextureBuffer::GetMipsLevel() const
{
	return m_MipsLevel;
}

uint32_t Lust::TextureBuffer::GetChannels() const
{
	return m_Channels;
}

Lust::TextureTensor Lust::TextureBuffer::GetTensor() const
{
	return m_Tensor;
}

const std::string& Lust::TextureBuffer::GetFilepath() const
{
	return m_Filepath;
}

void Lust::TextureBuffer::FreeImage()
{
	m_Image.reset();
}
