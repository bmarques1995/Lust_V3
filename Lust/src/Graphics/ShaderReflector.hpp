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

		const InputBufferLayout& GetInputLayout() const; // { return m_InputBufferLayout; }
		const SmallBufferLayout& GetSmallBufferLayout() const; // { return m_SmallBufferLayout; }
		const UniformLayout& GetUniformLayout() const; // { return m_UniformLayout; }
		const TextureLayout& GetTextureLayout() const; // { return m_TextureLayout; }
		const SamplerLayout& GetSamplerLayout() const; // { return m_SamplerLayout; }
		const StructuredBufferLayout& GetStructuredBufferLayout() const; // { return m_StructuredBufferLayout; }

		static ShaderReflector* Instantiate(std::string_view jsonBasepath, uint32_t stages, uint32_t numInstances = 1);
	
	protected:
		InputBufferLayout m_InputBufferLayout;
		SmallBufferLayout m_SmallBufferLayout;
		UniformLayout m_UniformLayout;
		StructuredBufferLayout m_StructuredBufferLayout;
		TextureLayout m_TextureLayout;
		SamplerLayout m_SamplerLayout;

		Json::Value m_PipelineInfo;
		std::string m_ShaderDir;

		uint32_t m_NumberOfInstances;

		void InitJsonAndPaths(std::string_view jsonFilepath);

		static const std::list<std::string> s_GraphicsPipelineStages;
	};
}