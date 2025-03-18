#pragma once
#include "Compiler.hpp"

namespace Lust
{
	/**
	* @brief SPV compiler, extends the compiler to compile Vulkan shaders
	*/
	class LUST_SHADER_MNG_API SPVCompiler : public Compiler
	{
	public:
		/**
		* @brief Constructor
		* @param baseEntry base entrypoint name
		* @param hlslFeatureLevel HLSL feature level
		* @param vulkanFeatureLevel Vulkan feature level
		*/
		SPVCompiler(std::string baseEntry = "_main", std::string hlslFeatureLevel = "_6_0", std::string vulkanFeatureLevel = "1.0");
		/**
		* @brief Destructor
		*/
		~SPVCompiler();

		/**
		* @brief Sets the Vulkan feature level, in the format "major.minor", e.g. "1.0"
		*/
		void SetVulkanFeatureLevel(std::string version);

		/**
		* @brief Compiles the shader, all the stages at once
		*/
		void CompilePackedShader();
		/**
		* @brief Pushes the arguments list for a shader stage
		*/
		void PushArgList(std::string stage);

	private:

		/**
		* @brief Validates the Vulkan feature level
		* @param version Vulkan feature level, in the format "major.minor", e.g. "1.0"
		*/
		void ValidateVulkanFeatureLevel(std::string version);
		
		/**
		* @brief Searches for a keyword in the list of keywords
		* @param value name of the keyword, first is the major and second the minor versions
		*/
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
