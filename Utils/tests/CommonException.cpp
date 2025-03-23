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
		EXPECT_STREQ("base exception", ge.what());
	}

	TEST(Utils, IsExceptionThrownWithEmptyMessage)
	{
		ExtendedGraphicsException ege("sample exception");
		EXPECT_STREQ("sample exception", ege.what());
	}
}