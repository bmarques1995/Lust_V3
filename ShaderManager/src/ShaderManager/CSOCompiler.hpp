#pragma once
#include "Compiler.hpp"

namespace Lust
{
	class LUST_SHADER_MNG_API CSOCompiler : public Compiler
	{
	public:
		CSOCompiler(std::string baseEntry = "_main", std::string hlslFeatureLevel = "_6_0");
		~CSOCompiler();

		//CSO/SPV
		void CompilePackedShader();
		
	private:
		void PushArgList(std::string stage);

		void PushRootSignatureArgs(std::string rs_name);
		void CompileRootSignature(std::string source, std::string basepath);

		std::wstring m_CurrentFormattedStage;
		std::wstring m_CurrentEntrypoint;
		std::string m_CurrentEntrypointCopy;

	};
}
