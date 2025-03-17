#pragma once

#include "LustUtilsDLLMacro.hpp"
#include <exception>
#include <string>

namespace Lust
{
	/**
	* @brief Exception class for graphics errors
	*/
	class LUST_UTILS_API GraphicsException : public std::exception
	{
	public:
		/**
		* @brief Constructor
		* @param reason The reason of the exception
		*/
		GraphicsException(std::string reason);

		/**
		* @brief Returns the reason of the exception
		* @return The reason of the exception
		*/
		char const* what() const override;
	protected:
		/**
		* @brief Default constructor
		*/
		GraphicsException();

		std::string m_Reason;
	};
}