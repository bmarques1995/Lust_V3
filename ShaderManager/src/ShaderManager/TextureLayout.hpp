#pragma once

#include "Image.hpp"
#include "LustShaderManagerDLLMacro.hpp"
#include <cstdint>
#include "CommonResourceDefs.hpp"

namespace Lust
{
	/**
	* @brief Enum for texture dimensions
	* @param TENSOR_1 represents a 1D texture
	* @param TENSOR_2 represents a 2D texture
	* @param TENSOR_3 represents a 3D texture
	*/
	enum class LUST_SHADER_MNG_API TextureTensor
	{
		TENSOR_1 = 1,
		TENSOR_2,
		TENSOR_3
	};

	/**
	* @brief %Texture element, represents a single texture handler
	*/
	class LUST_SHADER_MNG_API TextureElement
	{
	public:
		/**
		* @brief Default constructor
		*/
		TextureElement();
		/**
		* @brief Constructor
		* @param bindingSlot binding slot (Vulkan only), "binding", e.g. binding = 0
		* @param spaceSet descriptor set (Vulkan only), "set", e.g. set = 0
		* @param shaderRegister root signature element that describes the sampler (D3D12 only), "root Signature Index",
		* @param textureIndex offset in the root signature (D3D12 only), "texture Index", e.g. textureIndex = 0 means the first texture in the root signature
		*/
		TextureElement(uint32_t bindingSlot, uint32_t spaceSet, uint32_t shaderRegister, uint32_t textureIndex, std::string name);

		/**
		* @brief Returns the name of the texture
		*/
		const std::string& GetName() const;
		/**
		* @brief Returns the binding slot
		*/
		uint32_t GetBindingSlot() const;
		/**
		* @brief Returns the shader registerq
		*/
		uint32_t GetShaderRegister() const;
		/**
		* @brief Returns the space set
		*/
		uint32_t GetSpaceSet() const;
		/**
		* @brief Returns the texture index
		*/
		uint32_t GetTextureIndex() const;
		
	private:
		
		uint32_t m_SpaceSet;
		uint32_t m_BindingSlot;
		uint32_t m_ShaderRegister;
		uint32_t m_TextureIndex;
		std::string m_Name;
	};

	/**
	* @brief %Texture layout, represents a collection of single texture handlers
	*/
	class LUST_SHADER_MNG_API TextureLayout
	{
	public:
		/**
		* @brief Constructor
		* @param elements elements of the layout
		* @param allowedStages allowed stages
		*/
		TextureLayout(std::initializer_list<TextureElement> elements, uint32_t allowedStages);
		
		/**
		* @brief Returns the element of the layout
		* @param name name(key) of the element
		*/
		const TextureElement& GetElement(std::string name) const;
		/**
		* @brief Returns the elements of the layout
		*/
		const std::unordered_map<std::string, TextureElement>& GetElements() const;
		/**
		* @brief Returns the allowed stages
		*/
		uint32_t GetStages() const;
		
		/**
		* @brief Clears the layout
		*/
		void Clear();
		/**
		* @brief Uploads an element to the layout
		*/
		void Upload(const TextureElement& element);

	private:
		std::unordered_map<std::string, TextureElement> m_Textures;
		uint32_t m_Stages;
		static TextureElement s_EmptyElement;
	};

	/**
	* @brief %Texture array element, represents an array of textures handler
	*/
	class LUST_SHADER_MNG_API TextureArray
	{
	public:
		/**
		* @brief Default constructor
		*/
		TextureArray();
		/**
		* @brief Constructor
		* @param bindingSlot binding slot (Vulkan only), "binding", e.g. binding = 0
		* @param spaceSet descriptor set (Vulkan only), "set", e.g. set = 0
		* @param shaderRegister root signature element that describes the sampler (D3D12 only), "root Signature Index",
		* @param textureIndex offset in the root signature (D3D12 only), "texture Index", e.g. textureIndex = 0 means the first texture in the root signature
		* @param numberOfTextures number of textures
		* @param name name of the array
		*/
		TextureArray(uint32_t bindingSlot, uint32_t spaceSet, uint32_t shaderRegister, uint32_t textureIndex, uint32_t numberOfTextures, std::string name);

		/**
		* @brief Returns the name
		*/
		const std::string& GetName() const;
		/**
		* @brief Returns the binding slot
		*/
		uint32_t GetBindingSlot() const;
		/**
		* @brief Returns the shader register
		*/
		uint32_t GetShaderRegister() const;
		/**
		* @brief Returns the descriptor set
		*/
		uint32_t GetSpaceSet() const;
		/**
		* @brief Returns the texture index
		*/
		uint32_t GetTextureIndex() const;
		/**
		* @brief Returns the number of textures
		*/
		uint32_t GetNumberOfTextures() const;

	private:

		uint32_t m_SpaceSet;
		uint32_t m_BindingSlot;
		uint32_t m_ShaderRegister;
		uint32_t m_TextureIndex;
		uint32_t m_NumberOfTextures;
		std::string m_Name;
	};

	/**
	* @brief %Texture array layout, represents a collection of texture arrays
	*/
	class LUST_SHADER_MNG_API TextureArrayLayout
	{
	public:
		/**
		* @brief Constructor
		* @param elements elements of the layout
		* @param allowedStages allowed stages
		*/
		TextureArrayLayout(std::initializer_list<TextureArray> elements, uint32_t allowedStages);

		/**
		* @brief Returns the element of the layout
		* @param name name(key) of the element
		*/
		const TextureArray& GetElement(std::string name) const;
		/**
		* @brief Returns the elements of the layout
		*/
		const std::unordered_map<std::string, TextureArray>& GetElements() const;

		/**
		* @brief Returns the allowed stages
		*/
		uint32_t GetStages() const;

		/**
		* @brief Clears the layout
		*/
		void Clear();
		/**
		* @brief Uploads an element to the layout
		*/
		void Upload(const TextureArray& element);

	private:
		std::unordered_map<std::string, TextureArray> m_Textures;
		uint32_t m_Stages;
		static TextureArray s_EmptyElement;
	};

	/**
	* @brief %Texture buffer
	*/
	class LUST_SHADER_MNG_API TextureBuffer
	{
		friend class Texture2D;
		friend class VKTexture2D;
#ifdef LUST_SHADER_MNG_USES_WINDOWS
		friend class D3D12Texture2D;
#endif
	public:
		/**
		* @brief Default constructor
		*/
		TextureBuffer();
		/**
		* @brief Constructor
		* @param img image handler
		* @param tensor texture dimension
		*/
		TextureBuffer(std::shared_ptr<Image> img, TextureTensor tensor, std::string filepath, size_t depth = 1);
		/**
		* @brief Default destructor
		*/
		~TextureBuffer();

		/**
		* @brief Returns the texture buffer
		*/
		const uint8_t* GetTextureBuffer() const;
		/**
		* @brief Returns the width of the texture
		*/
		uint32_t GetWidth() const;
		/**
		* @brief Returns the height of the texture
		*/
		uint32_t GetHeight() const;
		/**
		* @brief Returns the depth of the texture
		*/
		uint32_t GetDepth() const;
		/**
		* @brief Returns the mips level, which is the log_2 of the greatest dimension
		*/
		uint32_t GetMipsLevel() const;
		/**
		* @brief Returns the number of channels
		*/
		uint32_t GetChannels() const;
		/**
		* @brief Returns the texture dimension
		*/
		TextureTensor GetTensor() const;
		/**
		* @brief Returns the image alignment
		*/
		ImageAlignment GetAlignment() const;

		/**
		* @brief Returns the filepath
		*/
		const std::string& GetFilepath() const;

		/**
		* @brief Frees the image buffer
		*/
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