#pragma once

#include "LustDLLMacro.hpp"
#include "InputBufferLayout.hpp"
#include "UniformsLayout.hpp"
#include "SamplerLayout.hpp"
#include "TextureLayout.hpp"
#include "StructuredBufferLayout.hpp"
#include <string>
#include <json/json.h>

namespace Lust
{
	class LUST_API ShaderReflector
	{
	public:
		ShaderReflector(uint32_t stages, uint32_t numInstances);
		virtual ~ShaderReflector() = default;

		const InputBufferLayout& GetInputLayout() const;
		const SmallBufferLayout& GetSmallBufferLayout() const;
		const UniformLayout& GetUniformLayout() const;
		const TextureLayout& GetTextureLayout() const;
		const SamplerLayout& GetSamplerLayout() const;
		const TextureArrayLayout& GetTextureArrayLayout() const;
		const SamplerArrayLayout& GetSamplerArrayLayout() const;
		const StructuredBufferLayout& GetStructuredBufferLayout() const;

		static ShaderReflector* Instantiate(std::string_view jsonBasepath, uint32_t stages, uint32_t numInstances = 1);
	
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

		void InitJsonAndPaths(std::string_view jsonFilepath);

		static const std::list<std::string> s_GraphicsPipelineStages;
	};
}