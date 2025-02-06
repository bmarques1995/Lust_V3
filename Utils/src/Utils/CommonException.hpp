#pragma once

#include "LustUtilsDLLMacro.hpp"
#include <exception>
#include <string>

namespace Lust
{
	class LUST_UTILS_API GraphicsException : public std::exception
	{
	public:
		GraphicsException(std::string reason);

		char const* what() const override;
	protected:
		GraphicsException();

		std::string m_Reason;
	};
}