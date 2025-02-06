#include "CompilerExceptions.hpp"

Lust::CompilerException::CompilerException(std::string reason) :
	m_Exception(reason)
{
}

const char* Lust::CompilerException::what() const
{
	return m_Exception.c_str();
}

Lust::InvalidNameException::InvalidNameException(std::string reason) :
	CompilerException(reason)
{
}

Lust::InvalidFilepathException::InvalidFilepathException(std::string reason) :
	CompilerException(reason)
{
}

Lust::InvalidPipelineException::InvalidPipelineException(std::string reason) :
	CompilerException(reason)
{
}

Lust::InvalidHLSLVersion::InvalidHLSLVersion(std::string reason) :
	CompilerException(reason)
{
}

Lust::InvalidVulkanVersion::InvalidVulkanVersion(std::string reason) :
	CompilerException(reason)
{
}
