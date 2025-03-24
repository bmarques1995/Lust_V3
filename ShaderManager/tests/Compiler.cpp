#include <gtest/gtest.h>
#include "SPVCompiler.hpp"
#include "BaseShader.hpp"
#include "FileHandler.hpp"
#include "Console.hpp"
#include "CompilerExceptions.hpp"

namespace Lust
{
	TEST(ShaderManager, InvalidPathForShader)
	{
		Console::Init();
		SPVCompiler compiler("_main", "_6_8", "1.3");
		ASSERT_THROW(compiler.PushShaderPath("./SampleShader.hl", PipelineType::Graphics), InvalidFilepathException);
		Console::End();
	}

	TEST(ShaderManager, InvalidVariablePattern)
	{
		Console::Init();
		SPVCompiler compiler("_main", "_6_8", "1.3");
		ASSERT_THROW(compiler.ValidateNameOfFunctionOrVariable("2main"), InvalidNameException);
		ASSERT_THROW(compiler.ValidateNameOfFunctionOrVariable("float4"), InvalidNameException);
		ASSERT_THROW(compiler.ValidateNameOfFunctionOrVariable("float4x4"), InvalidNameException);
		ASSERT_THROW(compiler.ValidateNameOfFunctionOrVariable("lerp"), InvalidNameException);
		ASSERT_THROW(compiler.ValidateNameOfFunctionOrVariable("SV_TARGET0"), InvalidNameException);
		Console::End();
	}

	TEST(ShaderManager, InvalidHLSLVersion)
	{
		Console::Init();
		SPVCompiler compiler("_main", "_6_8", "1.3");
		ASSERT_THROW(compiler.SetHLSLFeatureLevel("_7_3"), InvalidHLSLVersion);
		ASSERT_THROW(compiler.SetHLSLFeatureLevel("6.3"), InvalidHLSLVersion);
		ASSERT_NO_THROW(compiler.SetHLSLFeatureLevel("_6_8"));
		Console::End();
	}

	TEST(ShaderManager, InvalidFile)
	{
		Console::Init();
		SPVCompiler compiler("_main", "_6_8", "1.3");
		compiler.PushShaderPath("./SampleShaderRoot.hlsl", PipelineType::Graphics);
		ASSERT_THROW(compiler.CompilePackedShader(), InvalidFilepathException);
		Console::End();
	}
}
