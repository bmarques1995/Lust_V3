#pragma once

#include "LustShaderManagerDLLMacro.hpp"

#include <unordered_map>
#include <string>
#include "ComPointer.hpp"
#include "DXCSafeInclude.hpp"


namespace Lust
{
	/**
	* @enum PipelineStage
	* @brief Stages of the pipeline
	* @param VERTEX_STAGE represents the vertex stage
	* @param PIXEL_STAGE represents the fragment/pixel stage
	* @param GEOMETRY_STAGE represents the geometry stage
	* @param HULL_STAGE represents the tessellation control/hull stage
	* @param DOMAIN_STAGE represents the tessellation evaluation/domain stage
	* @param COMPUTE_STAGE represents the compute stage
	* @param MESH_STAGE represents the mesh stage
	* @param AMPLIFICATION_STAGE represents the task/amplification stage
	* @param RAYTRACING_STAGE represents the raytracing stage
	*/
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

	/**
	* @enum PipelineType
	* @brief Types of the pipeline
	* @param Graphics represents the graphics(vs + ps + gs + hs + ds) pipeline
	* @param Compute represents the compute(cs) pipeline
	* @param Mesh represents the mesh(ms + as + ps) pipeline
	* @param RayTracing represents the raytracing(lib) pipeline
	*/
	enum class LUST_SHADER_MNG_API PipelineType
	{
		Graphics,
		Compute,
		Mesh,
		RayTracing
	};

	/**
	* @brief %Shader compiler
	* @details Compiles a set of grouped shaders
	*/
	class LUST_SHADER_MNG_API Compiler
	{
	public:
		/**
		* @brief Sets the base entrypoint name, for example "_main" will be evaluated to "vs_main" for the vertex shader and "ps_main" for the pixel shader
		* @param baseEntry base entrypoint name
		*/
		void SetBaseEntry(std::string baseEntry);

		/**
		* @brief Adds a shader to the compilation queue
		* @details The compiler will compile all the shaders at once, in the order they are added
		* @param filepath filepath of the shader
		* @param pipelineType type of the pipeline
		*/
		void PushShaderPath(std::string filepath, PipelineType pipelineType);

		/**
		* @brief Sets the build mode: debug or release
		* @param isDebug debug mode
		*/
		void SetBuildMode(bool isDebug);

		/**
		* @brief Sets the HLSL feature level, in the format "_major_minor"
		* @param version HLSL feature level
		*/
		void SetHLSLFeatureLevel(std::string version);

		/**
		* @brief Validates the name of the variable
		* @param name name of the variable
		*/
		void ValidateNameOfFunctionOrVariable(std::string name);

	protected:
		/**
		* @brief Constructor
		* @param backendExtension extension of the backend (.spv/.cso)
		* @param graphicsAPIExtension extension of the graphics API (.vk/.d3d12)
		* @param baseEntry base entrypoint name
		* @param hlslFeatureLevel HLSL feature level
		*/
		Compiler(std::string_view backendExtension, std::string_view graphicsAPIExtension, std::string baseEntry = "_main", std::string hlslFeatureLevel = "_6_0");
		/**
		* @brief Destructor
		*/
		virtual ~Compiler();

		/**
		* @brief Compiles a shader stage
		* @param source source code of the shader
		* @param stage stage of the shader
		* @param basepath base path of the shader
		* @return true if the compilation was successful
		*/
		bool CompileStage(std::string source, std::string stage, std::string basepath);
		/**
		* @brief Reads the source code of a shader
		* @param path filepath of the shader
		* @param shaderSource source code of the shader
		*/
		void ReadShaderSource(std::string_view path, std::string* shaderSource);

		/**
		* @brief Searches for a keyword in the list of keywords
		* @param value name of the keyword
		* @return iterator to the keyword
		*/
		std::list<std::string>::const_iterator SearchBuiltinName(std::string value);
		/**
		* @brief Searches for a keyword in the list of keywords
		* @param value name of the keyword
		* @return iterator to the keyword
		*/
		std::list<std::pair<uint32_t, uint32_t>>::const_iterator SearchHLSLVersion(std::pair<uint32_t, uint32_t> value);
		
		/**
		* @brief Validates the HLSL feature level
		* @param version HLSL feature level
		*/
		void ValidateHLSLFeatureLevel(std::string version);
		
		/**
		* @brief Validates the name of the shader
		* @param name name of the variable
		*/
		void ValidateNameOverKeywords(std::string name);
		/**
		* @brief Validates the name of the shader
		* @param name name of the variable
		*/
		void ValidateNameOverSysValues(std::string name);
		/**
		* @brief Validates the name of the shader
		* @param name name of the variable
		*/
		void ValidateNameOverBuiltinFunctions(std::string name);
		/**
		* @brief Validates the name of the shader
		* @param name name of the variable
		* @details Validates the pipeline stages and if the pipeline type is correct, for the graphics pipeline, vertex and pixel shader stages are required,
		* geometry shader is optional and hull is required if domain is present, domain is optional.
		* For the compute pipeline, compute shader is required.
		* For the raytracing pipeline, raytracing shader is required.
		* For the mesh pipeline, mesh shader and pixel shader are required, amplification shader is optional.
		*/
		bool ValidatePipeline(uint32_t stages, PipelineType pipelineType);
		
		/**
		* @brief Pushes the present stages of a shader
		* @param shaderPath filepath of the shader
		* @param shader hlsl frontend of the shader
		* @param m_Stages [Out] Stages of the shader
		* @details This method is used to compile only the stages that are present
		*/
		void PushPresentStages(std::string shaderPath, std::string shader, std::vector<std::pair<std::string, Lust::PipelineStage>>* m_Stages);

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