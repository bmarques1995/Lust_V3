#pragma once

#include "LustShaderManagerDLLMacro.hpp"

#include <unordered_map>
#include <string>
#include "ComPointer.hpp"
#include "DXCSafeInclude.hpp"


namespace Lust
{
	enum class LUST_SHADER_MNG_API PipelineStage
	{
		VERTEX_STAGE = 1,
		PIXEL_STAGE = 2,
		GEOMETRY_STAGE = 4,
		HULL_STAGE = 8,
		DOMAIN_STAGE = 16,
		COMPUTE_STAGE = 32,
		MESH_STAGE = 64,
		AMPLIFICATION_STAGE = 128,
		RAYTRACING_STAGE = 256
	};

	enum class LUST_SHADER_MNG_API PipelineType
	{
		Graphics,
		Compute,
		Mesh,
		RayTracing
	};

	class LUST_SHADER_MNG_API Compiler
	{
	public:
		
		void SetBaseEntry(std::string baseEntry);

		void PushShaderPath(std::string filepath, PipelineType pipelineType);

		void SetBuildMode(bool isDebug);

		void SetHLSLFeatureLevel(std::string version);

	protected:
		Compiler(std::string_view backendExtension, std::string_view graphicsAPIExtension, std::string baseEntry = "_main", std::string hlslFeatureLevel = "_6_0");
		~Compiler();

		bool CompileStage(std::string source, std::string stage, std::string basepath);
		void ReadShaderSource(std::string_view path, std::string* shaderSource);

		std::list<std::string>::const_iterator SearchBuiltinName(std::string value);
		std::list<std::pair<uint32_t, uint32_t>>::const_iterator SearchHLSLVersion(std::pair<uint32_t, uint32_t> value);
		

		void ValidateHLSLFeatureLevel(std::string version);
		
		void ValidateNameOverKeywords(std::string name);
		void ValidateNameOverSysValues(std::string name);
		void ValidateNameOverBuiltinFunctions(std::string name);
		bool ValidatePipeline(uint32_t stages, PipelineType pipelineType);
		
		static const std::unordered_map<std::string, bool> s_Keywords;
		static const std::unordered_map<std::string, bool> s_SysValues;
		static const std::list<std::string> s_BuiltinFunctions;
		static const std::list<std::pair<uint32_t, uint32_t>> s_ValidHLSL;
		static const std::unordered_map<std::string, PipelineStage> s_ShaderStages;

		std::vector<std::pair<std::string, PipelineType>> m_ShaderFilepaths;
		
		std::vector<const wchar_t*> m_ArgList;

		bool m_PackedShaders;
		bool m_DebugMode;
		
		//HLSLBackend m_Backend;
		std::string m_BaseEntry;
		std::string m_HLSLFeatureLevel;
		std::string m_GraphicsAPIExtension;
		std::string m_BackendExtension;
	};
}