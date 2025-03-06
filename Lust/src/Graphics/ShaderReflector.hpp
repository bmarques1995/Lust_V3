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
		virtual ~ShaderReflector() = default;

		void ExportInputLayout(InputBufferLayout* inputBufferLayout);
		static ShaderReflector* Instantiate(std::string_view jsonBasepath);
	
	protected:
		InputBufferLayout m_InputBufferLayout;

		Json::Value m_PipelineInfo;
		std::string m_ShaderDir;

		void InitJsonAndPaths(std::string_view jsonFilepath);

		static const std::list<std::string> s_GraphicsPipelineStages;
	};
}