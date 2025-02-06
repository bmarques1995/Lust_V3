#pragma once
#include "Compiler.hpp"

namespace Lust
{
	class LUST_SHADER_MNG_API SPVCompiler : public Compiler
	{
	public:
		SPVCompiler(std::string baseEntry = "_main", std::string hlslFeatureLevel = "_6_0", std::string vulkanFeatureLevel = "1.0");
		~SPVCompiler();

		//SPV
		void SetVulkanFeatureLevel(std::string version);

		//CSO/SPV
		void CompilePackedShader();
		void PushArgList(std::string stage);

	private:

		void ValidateVulkanFeatureLevel(std::string version);
		
		

		//SPV
		std::list<std::pair<uint32_t, uint32_t>>::const_iterator SearchVulkanVersion(std::pair<uint32_t, uint32_t> value);

		//SPV
		std::string m_VulkanFeatureLevel;

		std::wstring m_CurrentFormattedStage;
		std::wstring m_CurrentEntrypoint;
		std::string m_CurrentEntrypointCopy;
		std::wstring m_VulkanFeatureLevelArg;

		//SPV
		static const std::list<std::pair<uint32_t, uint32_t>> s_ValidVulkan;
	};
}
