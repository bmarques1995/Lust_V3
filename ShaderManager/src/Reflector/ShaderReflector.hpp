#pragma once

#include "LustShaderManagerDLLMacro.hpp"
#include "InputBufferLayout.hpp"
#include "UniformsLayout.hpp"
#include "SamplerLayout.hpp"
#include "TextureLayout.hpp"
#include "StructuredBufferLayout.hpp"
#include "TargetAPI.hpp"
#include <string>
#include <json/json.h>

namespace Lust
{
	/**
	* @brief %Shader reflector
	* @details Contains the shader reflector, which is used to reflect the shader, in other words, to get the layout of the shader,
	* of all the inputs, uniforms, textures, samplers, structured buffers, and the input assembly layout
	*/
	class LUST_SHADER_MNG_API ShaderReflector
	{
	public:
		/**
		* @brief Constructor
		* @param stages The number of stages
		* @param numInstances The number of instances
		*/
		ShaderReflector(uint32_t stages, uint32_t numInstances);
		/**
		* @brief Destructor
		*/
		virtual ~ShaderReflector() = default;

		/**
		* @brief Gets the input layout
		*/
		const InputBufferLayout& GetInputLayout() const;
		/**
		* @brief Gets the small buffer layout
		*/
		const SmallBufferLayout& GetSmallBufferLayout() const;
		/**
		* @brief Gets the uniform layout
		*/
		const UniformLayout& GetUniformLayout() const;
		/**
		* @brief Gets the texture layout
		*/
		const TextureLayout& GetTextureLayout() const;
		/**
		* @brief Gets the sampler layout
		*/
		const SamplerLayout& GetSamplerLayout() const;
		/**
		* @brief Gets the texture array layout
		*/
		const TextureArrayLayout& GetTextureArrayLayout() const;
		/**
		* @brief Gets the sampler array layout
		*/
		const SamplerArrayLayout& GetSamplerArrayLayout() const;
		/**
		* @brief Gets the structured buffer layout
		*/
		const StructuredBufferLayout& GetStructuredBufferLayout() const;

		/**
		* @brief Instantiates a shader reflector
		* @param jsonBasepath The json base path
		* @param stages The number of stages
		* @param numInstances The number of instances
		*/
		static ShaderReflector* Instantiate(std::string_view jsonBasepath, uint32_t stages, uint32_t numInstances = 1);
	
		/**
		* @brief Init the infos of the api
		* @param uniformAttachment uniform attachment
		* @param smallBufferAttachment uniform attachment
		* @param api current graphics api
		*/
		static void InitAPIInfos(uint32_t uniformAttachment, uint32_t smallBufferAttachment, TargetAPI api);

	protected:
		InputBufferLayout m_InputBufferLayout;
		SmallBufferLayout m_SmallBufferLayout;
		UniformLayout m_UniformLayout;
		StructuredBufferLayout m_StructuredBufferLayout;
		TextureLayout m_TextureLayout;
		SamplerLayout m_SamplerLayout;
		TextureArrayLayout m_TextureArrayLayout;
		SamplerArrayLayout m_SamplerArrayLayout;

		Json::Value m_PipelineInfo;
		std::string m_ShaderDir;

		uint32_t m_NumberOfInstances;
		

		/**
		* @brief Initializes the json and paths
		* @param jsonFilepath The json file path, with the extensions
		*/
		void InitJsonAndPaths(std::string_view jsonFilepath);

		static const std::list<std::string> s_GraphicsPipelineStages;

		static uint32_t GetUniformAttachment();
		static uint32_t GetSmallBufferAttachment();

	private:
		static uint32_t s_UniformAttachment;
		static uint32_t s_SmallBufferAttachment;
		static TargetAPI s_API;
		static bool s_InitializedAttachments;
	};
}