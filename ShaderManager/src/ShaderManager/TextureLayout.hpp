#pragma once

#include "Image.hpp"
#include "LustShaderManagerDLLMacro.hpp"
#include <cstdint>

namespace Lust
{
	enum class LUST_SHADER_MNG_API TextureTensor
	{
		TENSOR_1 = 1,
		TENSOR_2,
		TENSOR_3
	};

	class LUST_SHADER_MNG_API TextureElement
	{
	public:
		TextureElement();
		TextureElement(uint32_t bindingSlot, uint32_t spaceSet, uint32_t shaderRegister, uint32_t textureIndex, std::string name);

		const std::string& GetName() const;
		uint32_t GetBindingSlot() const;
		uint32_t GetShaderRegister() const;
		uint32_t GetSpaceSet() const;
		uint32_t GetTextureIndex() const;
		
	private:
		
		uint32_t m_SpaceSet;
		uint32_t m_BindingSlot;
		uint32_t m_ShaderRegister;
		uint32_t m_TextureIndex;
		std::string m_Name;
	};

	class LUST_SHADER_MNG_API TextureLayout
	{
	public:
		TextureLayout(std::initializer_list<TextureElement> elements, uint32_t allowedStages);

		const TextureElement& GetElement(std::string name) const;
		const std::unordered_map<std::string, TextureElement>& GetElements() const;

		uint32_t GetStages() const;

		void Clear();
		void Upload(const TextureElement& element);

	private:
		std::unordered_map<std::string, TextureElement> m_Textures;
		uint32_t m_Stages;
		static TextureElement s_EmptyElement;
	};

	class LUST_SHADER_MNG_API TextureArray
	{
	public:
		TextureArray();
		TextureArray(uint32_t bindingSlot, uint32_t spaceSet, uint32_t shaderRegister, uint32_t textureIndex, uint32_t numberOfTextures, std::string name);

		const std::string& GetName() const;
		uint32_t GetBindingSlot() const;
		uint32_t GetShaderRegister() const;
		uint32_t GetSpaceSet() const;
		uint32_t GetTextureIndex() const;
		uint32_t GetNumberOfTextures() const;

	private:

		uint32_t m_SpaceSet;
		uint32_t m_BindingSlot;
		uint32_t m_ShaderRegister;
		uint32_t m_TextureIndex;
		uint32_t m_NumberOfTextures;
		std::string m_Name;
	};

	class LUST_SHADER_MNG_API TextureArrayLayout
	{
	public:
		TextureArrayLayout(std::initializer_list<TextureArray> elements, uint32_t allowedStages);

		const TextureArray& GetElement(std::string name) const;
		const std::unordered_map<std::string, TextureArray>& GetElements() const;

		uint32_t GetStages() const;

		void Clear();
		void Upload(const TextureArray& element);

	private:
		std::unordered_map<std::string, TextureArray> m_Textures;
		uint32_t m_Stages;
		static TextureArray s_EmptyElement;
	};

	class LUST_SHADER_MNG_API TextureBuffer
	{
		friend class Texture2D;
		friend class VKTexture2D;
#ifdef LUST_SHADER_MNG_USES_WINDOWS
		friend class D3D12Texture2D;
#endif
	public:
		TextureBuffer();
		TextureBuffer(std::shared_ptr<Image> img, TextureTensor tensor, std::string filepath, size_t depth = 1);
		~TextureBuffer();

		const uint8_t* GetTextureBuffer() const;
		uint32_t GetWidth() const;
		uint32_t GetHeight() const;
		uint32_t GetDepth() const;
		uint32_t GetMipsLevel() const;
		uint32_t GetChannels() const;
		TextureTensor GetTensor() const;
		ImageAlignment GetAlignment() const;

		const std::string& GetFilepath() const;

		void FreeImage();
	private:
		std::shared_ptr<Image> m_Image;
		TextureTensor m_Tensor;
		size_t m_Depth;
		uint32_t m_Width;
		uint32_t m_Height;
		uint32_t m_MipsLevel;
		uint32_t m_Channels;
		ImageAlignment m_ImageAlignment;
		std::string m_Filepath;
	};
}