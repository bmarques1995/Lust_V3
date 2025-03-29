#ifdef LUST_SHADER_MNG_USES_WINDOWS

#include <gtest/gtest.h>
#include "CSOCompiler.hpp"
#include "BaseShader.hpp"
#include "FileHandler.hpp"
#include "Console.hpp"
#include "CompilerExceptions.hpp"

namespace Lust
{
	TEST(CSOCompiler, IsCSOShaderCompilerWorkingForGraphicsPipeline)
	{
		Console::Init();
		CSOCompiler compiler("main", "_6_8");
		compiler.SetBaseEntry("_main");
		FileHandler::WriteTextFile("SampleShader.hlsl", GraphicsFullShader);
		compiler.PushShaderPath("./SampleShader.hlsl", PipelineType::Graphics);
		compiler.CompilePackedShader();
		ASSERT_TRUE(FileHandler::FileExists("SampleShader.d3d12.json"));
		ASSERT_TRUE(FileHandler::FileExists("SampleShader.rs.cso"));
		ASSERT_TRUE(FileHandler::FileExists("SampleShader.vs.cso"));
		ASSERT_TRUE(FileHandler::FileExists("SampleShader.ps.cso"));
		std::remove("SampleShader.hlsl");
		std::remove("SampleShader.d3d12.json");
		std::remove("SampleShader.rs.cso");
		std::remove("SampleShader.vs.cso");
		std::remove("SampleShader.ps.cso");
		Console::End();
	}

	TEST(CSOCompiler, IsCSOShaderCompilerDetectingInvalidPipeline)
	{
		Console::Init();
		CSOCompiler compiler("main", "_6_8");
		compiler.SetBaseEntry("_main");
		FileHandler::WriteTextFile("SampleShaderDefective.hlsl", GraphicsFullShaderMissingPixel);
		compiler.SetBuildMode(false);
		compiler.PushShaderPath("./SampleShaderDefective.hlsl", PipelineType::Graphics);
		ASSERT_THROW(compiler.CompilePackedShader(), InvalidPipelineException);
		std::remove("SampleShaderDefective.hlsl");
		Console::End();
	}

	TEST(CSOCompiler, IsCSOShaderCompilerDetectingCompilationError)
	{
		Console::Init();
		CSOCompiler compiler("main", "_6_8");
		compiler.SetBaseEntry("_main");
		FileHandler::WriteTextFile("SampleShaderDefective.hlsl", GraphicsPixelShaderError);
		compiler.SetBuildMode(false);
		compiler.PushShaderPath("./SampleShaderDefective.hlsl", PipelineType::Graphics);
		ASSERT_THROW(compiler.CompilePackedShader(), InvalidPipelineException);
		std::remove("SampleShaderDefective.hlsl");
		Console::End();
	}

	TEST(CSOCompiler, IsCSOShaderCompilerDetectingInvalidRootSignature)
	{
		Console::Init();
		CSOCompiler compiler("main", "_6_8");
		compiler.SetBaseEntry("_main");
		FileHandler::WriteTextFile("SampleShaderDefective.hlsl", GraphicsRootSignatureError);
		compiler.SetBuildMode(false);
		compiler.PushShaderPath("./SampleShaderDefective.hlsl", PipelineType::Graphics);
		ASSERT_THROW(compiler.CompilePackedShader(), InvalidPipelineException);
		std::remove("SampleShaderDefective.hlsl");
		Console::End();
	}
}

#endif