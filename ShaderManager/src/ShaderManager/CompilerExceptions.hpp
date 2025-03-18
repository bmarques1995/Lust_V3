#pragma once

#include "LustShaderManagerDLLMacro.hpp"
#include <exception>
#include <string>

namespace Lust
{
	/**
	* @brief Base class for all compiler exceptions
	*/
	class LUST_SHADER_MNG_API CompilerException : public std::exception
	{
	protected:
		CompilerException(std::string reason);

		std::string m_Exception;
	public:
		const char* what() const override;
	};

	/**
	* @brief Exception for invalid names
	*/
	class LUST_SHADER_MNG_API InvalidNameException : public CompilerException
	{
	public:
		InvalidNameException(std::string reason);
	};

	/**
	* @brief Exception for invalid pipelines
	*/
	class LUST_SHADER_MNG_API InvalidPipelineException : public CompilerException
	{
	public:
		InvalidPipelineException(std::string reason);
	};

	/**
	* @brief Exception for invalid filepaths
	*/
	class LUST_SHADER_MNG_API InvalidFilepathException : public CompilerException
	{
	public:
		InvalidFilepathException(std::string reason);
	};

	/**
	* @brief Exception for invalid HLSL versions
	*/
	class LUST_SHADER_MNG_API InvalidHLSLVersion : public CompilerException
	{
	public:
		InvalidHLSLVersion(std::string reason);
	};

	/**
	* @brief Exception for invalid Vulkan versions
	*/
	class LUST_SHADER_MNG_API InvalidVulkanVersion : public CompilerException
	{
	public:
		InvalidVulkanVersion(std::string reason);
	};
}