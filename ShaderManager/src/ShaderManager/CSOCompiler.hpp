#pragma once

#ifdef LUST_SHADER_MNG_USES_WINDOWS

#include "Compiler.hpp"

namespace Lust
{
	/**
	* @brief CSO compiler, extends the compiler to compile D3D12 shaders
	* @note This class is only available on Windows
	*/
	class LUST_SHADER_MNG_API CSOCompiler : public Compiler
	{
	public:
		/**
		* @brief Constructor
		* @param baseEntry base entrypoint name
		* @param hlslFeatureLevel HLSL feature level
		*/
		CSOCompiler(std::string baseEntry = "_main", std::string hlslFeatureLevel = "_6_0");
		/**
		* @brief Destructor
		*/
		~CSOCompiler();

		/**
		* @brief Compiles the shader
		* @details Compiles all the shaders at once
		*/
		void CompilePackedShader();
		
	private:
		/**
		* @brief Pushes the arguments list for a shader stage
		* @param stage stage of the shader
		*/
		void PushArgList(std::string stage);

		/**
		* @brief Pushes the arguments list for a root signature
		* @param rs_name name of the root signature
		*/
		void PushRootSignatureArgs(std::string rs_name);
		/**
		* @brief Compiles a root signature
		* @param source source code of the shader
		* @param basepath base path of the shader
		*/
		void CompileRootSignature(std::string source, std::string basepath);

		std::wstring m_CurrentFormattedStage;
		std::wstring m_CurrentEntrypoint;
		std::string m_CurrentEntrypointCopy;

	};
}

#endif
