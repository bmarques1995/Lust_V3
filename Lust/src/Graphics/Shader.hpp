#pragma once

#include "LustDLLMacro.hpp"
#include "InputBufferLayout.hpp"
#include "UniformsLayout.hpp"
#include "SamplerLayout.hpp"
#include "TextureLayout.hpp"
#include "StructuredBufferLayout.hpp"
#include "Topology.hpp"
#include "Texture.hpp"
#include "Sampler.hpp"
#include "Buffer.hpp"
#include "GraphicsContext.hpp"
#include <json/json.h>


namespace Lust
{
	/**
	* @brief Exception for shader size mismatch, where the provided buffer size does not match the expected size
	*/
	class LUST_API SizeMismatchException : public GraphicsException
	{
	public:
		SizeMismatchException(size_t layoutSize, size_t providedSize);
	};

	/**
	* @brief %Shader input information
	* @details Contains the shader input information
	* @param m_InputLayout The input layout
	* @param m_SmallBufferLayout The small buffer layout
	* @param m_UniformLayout The uniform layout
	* @param m_TextureLayout The texture layout
	* @param m_SamplerLayout The sampler layout
	* @param m_TextureArrayLayout The texture array layout
	* @param m_SamplerArrayLayout The sampler array layout
	* @param m_StructuredBufferLayout The structured buffer layout
	* @param m_Topology The topology
	*/
	struct LUST_API InputInfo
	{
		InputBufferLayout m_InputLayout;
		SmallBufferLayout m_SmallBufferLayout;
		UniformLayout m_UniformLayout;
		TextureLayout m_TextureLayout;
		SamplerLayout m_SamplerLayout;
		TextureArrayLayout m_TextureArrayLayout;
		SamplerArrayLayout m_SamplerArrayLayout;
		StructuredBufferLayout m_StructuredBufferLayout;
		Topology m_Topology = Topology::LUST_TOPOLOGY_TRIANGLELIST;
		
		/**
		* @brief Constructor
		* @details Creates the shader input information
		* @param stages The stages present in the usage
		*/
		InputInfo(uint32_t stages);
		/**
		* @brief Constructor
		* @details Creates the shader input information
		* @param inputLayout The input layout
		* @param smallBufferLayout The small buffer layout
		* @param uniformLayout The uniform layout
		* @param textureLayout The texture layout
		* @param samplerLayout The sampler layout
		* @param textureArrayLayout The texture array layout
		* @param samplerArrayLayout The sampler array layout
		* @param structuredBufferLayout The structured buffer layout
		*/
		InputInfo(InputBufferLayout inputLayout, SmallBufferLayout smallBufferLayout, UniformLayout uniformLayout, TextureLayout textureLayout, SamplerLayout samplerLayout, 
			TextureArrayLayout textureArrayLayout, SamplerArrayLayout samplerArrayLayout, StructuredBufferLayout structuredBufferLayout);
	};

	/**
	* @brief %Shader base class, handles the graphics pipeline shader
	*/
	class LUST_API Shader
	{
	public:
		/**
		* @brief Constructor
		* @param inputInfo The shader input information
		* @param filepath The shader filepath
		*/
		Shader(const InputInfo& inputInfo, const std::string& filepath);

		/**
		* @brief Returns the InputBufferLayout
		*/
		const InputBufferLayout& GetInputLayout() const;
		/**
		* @brief Returns the SmallBufferLayout
		*/
		const SmallBufferLayout& GetSmallBufferLayout() const;
		/**
		* @brief Returns the UniformLayout
		*/
		const UniformLayout& GetUniformLayout() const;
		/**
		* @brief Returns the StructuredBufferLayout
		*/
		const StructuredBufferLayout& GetStructuredBufferLayout() const;
		/**
		* @brief Returns the TextureLayout
		*/
		const TextureLayout& GetTextureLayout() const;
		/**
		* @brief Returns the SamplerLayout
		*/
		const SamplerLayout& GetSamplerLayout() const;
		/**
		* @brief Returns the TextureArrayLayout
		*/
		const TextureArrayLayout& GetTextureArrayLayout() const;
		/**
		* @brief Returns the SamplerArrayLayout
		*/
		const SamplerArrayLayout& GetSamplerArrayLayout() const;

		/**
		* @brief Destructor
		*/
		virtual ~Shader() = default;
		/**
		* @brief Stage the shader, which tells that the shader is ready to be used
		*/
		virtual void Stage() = 0;
		/**
		* @brief Returns the InputBufferLayout stride
		*/
		virtual uint32_t GetStride() const = 0;
		/**
		* Returns an element offset
		*/
		virtual uint32_t GetOffset() const = 0;

		/**
		* @brief Returns the name of the shader
		*/
		const std::string& GetName() const;
		/**
		* @brief Sets the name of the shader
		*/
		void SetName(const std::string& name);

		/**
		* @brief Uploads a texture 2D
		* @param texture The texture
		* @param textureElement The texture element
		*/
		virtual void UploadTexture2D(const std::shared_ptr<Texture2D>* texture, const TextureElement& textureElement) = 0;
		/**
		* @brief Uploads a texture 2D
		* @param texture The texture
		* @param textureArray The texture array
		* @param offset The offset, that is the index of the texture in the array
		*/
		virtual void UploadTexture2D(const std::shared_ptr<Texture2D>* texture, const TextureArray& textureArray, uint32_t offset) = 0;
		/**
		* @brief Uploads a constant buffer
		* @param buffer The constant buffer
		* @param uploadCBV The constant buffer element
		*/
		virtual void UploadConstantBuffer(const std::shared_ptr<UniformBuffer>* buffer, const UniformElement& uploadCBV) = 0;
		/**
		* @brief Uploads a structured buffer
		* @param buffer The structured buffer
		* @param uploadSRV The structured buffer element
		*/
		virtual void UploadStructuredBuffer(const std::shared_ptr<StructuredBuffer>* buffer, const StructuredBufferElement& uploadSRV) = 0;

		/**
		* @brief Uploads a sampler
		* @param sampler The sampler
		* @param samplerElement The sampler element
		*/
		virtual void UploadSampler(const std::shared_ptr<Sampler>* sampler, const SamplerElement& samplerElement) = 0;
		/**
		* @brief Uploads a sampler
		* @param sampler The sampler
		* @param samplerArray The sampler array
		* @param offset The offset, that is the index of the sampler in the array
		*/
		virtual void UploadSampler(const std::shared_ptr<Sampler>* sampler, const SamplerArray& samplerArray, uint32_t offset) = 0;

		/**
		* @brief Binds a small buffer
		* @param data The data to bind
		* @param size The size of the data
		* @param smallBuffer The small buffer element
		*/
		virtual void BindSmallBuffer(const void* data, size_t size, const SmallBufferElement& smallBuffer, size_t offset) = 0;
		/**
		* @brief Binds all descriptors set/heap at once
		*/
		virtual void BindDescriptors() = 0;

		/**
		* @brief Uploads a texture array desc set, this method only used by Vulkan
		*/
		virtual void UploadTexturePackedDescSet(const TextureArray& textureArray) = 0;
		/**
		* @brief Uploads a sampler array desc set, this method only used by Vulkan
		*/
		virtual void UploadSamplerPackedDescSet(const SamplerArray& samplerArray) = 0;

		/**
		* @brief Instantiates a shader
		* @param context The graphics context
		* @param json_basepath The json base path is a path, without extensions, where the json file is located, is completed with the json extension based on the graphics API
		* @param inputInfo The input info
		*/
		static Shader* Instantiate(const GraphicsContext* context, std::string json_basepath, const InputInfo& inputInfo);

	protected:
		Json::Value m_PipelineInfo;
		std::string m_ShaderDir;
		std::string m_Name;

		InputBufferLayout m_Layout;
		SmallBufferLayout m_SmallBufferLayout;
		UniformLayout m_UniformLayout;
		TextureLayout m_TextureLayout;
		SamplerLayout m_SamplerLayout;
		TextureArrayLayout m_TextureArrayLayout;
		SamplerArrayLayout m_SamplerArrayLayout;
		StructuredBufferLayout m_StructuredBufferLayout;

		/**
		* @brief Initializes the json and paths
		* @param json_controller_path The json controller path, that is the json file that controls the pipeline, now with extensions: `.{vk,d3d12}.json`
		* @param pipelineInfo The pipeline info
		*/
		static void InitJsonAndPaths(std::string json_controller_path, Json::Value* pipelineInfo, std::string* shaderDir);
	};

	/**
	* @brief %Shader library, a library of shaders
	*/
	class LUST_API ShaderLibrary
	{
	public:
		/**
		* @brief Constructor
		* @param context The graphics context
		*/
		ShaderLibrary(const GraphicsContext* context);
		/**
		* @brief Destructor
		*/
		~ShaderLibrary();

		/**
		* @brief Adds a shader to the library
		* @param name The name of the shader
		* @param shader The shader
		*/
		void Add(const std::string& name, const std::shared_ptr<Shader>& shader);
		/**
		* @brief Adds a shader to the library
		*/
		void Add(const std::shared_ptr<Shader>& shader);
		/**
		* @brief Loads a shader
		* @param json_basepath The json base path is a path, without extensions, where the json file is located, is completed with the json extension based on the graphics API
		* @param inputInfo The input info
		*/
		std::shared_ptr<Shader> Load(const std::string& json_basepath, const InputInfo& inputInfo);
		/**
		* @brief Loads a shader
		* @param name The name of the shader
		* @param json_basepath The json base path is a path, without extensions, where the json file is located, is completed with the json extension based on the graphics API
		* @param inputInfo The input info
		*/
		std::shared_ptr<Shader> Load(const std::string& name, const std::string& json_basepath, const InputInfo& inputInfo);

		/**
		* @brief Gets a shader
		* @param name The name of the shader
		*/
		std::shared_ptr<Shader> Get(const std::string& name);

		/**
		* @brief Checks if a shader exists
		*/
		bool Exists(const std::string& name) const;
	private:
		std::unordered_map<std::string, std::shared_ptr<Shader>> m_Shaders;
		const GraphicsContext* m_Context;
	};
}