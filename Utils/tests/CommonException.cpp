#include <gtest/gtest.h>
#include "CommonException.hpp"


namespace Lust
{
	class ExtendedGraphicsException : public GraphicsException
	{
	public:
		ExtendedGraphicsException(std::string reason) : GraphicsException()
		{
			m_Reason = reason;
		}
	};

	TEST(Utils, IsExceptionThrownWithCorrectMessage)
	{
		GraphicsException ge("base exception");
		ASSERT_STREQ("base exception", ge.what());
	}

	TEST(Utils, IsExceptionThrownWithEmptyMessage)
	{
		ExtendedGraphicsException ege("sample exception");
		ASSERT_STREQ("sample exception", ege.what());
	}
}