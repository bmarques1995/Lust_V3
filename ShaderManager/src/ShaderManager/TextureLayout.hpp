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
		friend class Texture2D;
		friend class VKTexture2D;
#ifdef LUST_SHADER_MNG_USES_WINDOWS
		friend class D3D12Texture2D;
#endif
	public:
		TextureElement();
		TextureElement(std::shared_ptr<Image> img, uint32_t bindingSlot, uint32_t shaderRegister, uint32_t spaceSet, TextureTensor tensor, uint32_t textureIndex, size_t depth = 1);

		const uint8_t* GetTextureBuffer() const;
		uint32_t GetWidth() const;
		uint32_t GetHeight() const;
		uint32_t GetDepth() const;
		uint32_t GetMipsLevel() const;
		uint32_t GetChannels() const;
		TextureTensor GetTensor() const;
		uint32_t GetBindingSlot() const;
		uint32_t GetShaderRegister() const;
		uint32_t GetSpaceSet() const;
		uint32_t GetTextureIndex() const;
		void FreeImage();
	private:
		TextureTensor m_Tensor;
		std::shared_ptr<Image> m_Image;
		size_t m_Depth;
		uint32_t m_Width;
		uint32_t m_Height;
		uint32_t m_MipsLevel;
		uint32_t m_Channels;
		uint32_t m_SpaceSet;
		uint32_t m_BindingSlot;
		uint32_t m_ShaderRegister;
		uint32_t m_TextureIndex;
	};

	class LUST_SHADER_MNG_API TextureLayout
	{
	public:
		TextureLayout(std::initializer_list<TextureElement> elements, uint32_t allowedStages);

		const TextureElement& GetElement(uint32_t shaderRegister, uint32_t textureIndex) const;
		const std::unordered_map<uint64_t, TextureElement>& GetElements() const;

		uint32_t GetStages() const;

	private:
		std::unordered_map<uint64_t, TextureElement> m_Textures;
		uint32_t m_Stages;
		static TextureElement s_EmptyElement;
	};
}