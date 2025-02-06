#include "CommonException.hpp"

char const* Lust::GraphicsException::what() const
{
	return m_Reason.c_str();
}

Lust::GraphicsException::GraphicsException() :
	m_Reason("")
{

}

Lust::GraphicsException::GraphicsException(std::string reason) :
	m_Reason(reason)
{

}