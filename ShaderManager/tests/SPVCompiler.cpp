#include <gtest/gtest.h>
#include "SPVCompiler.hpp"
#include "BaseShader.hpp"
#include "FileHandler.hpp"
#include "Console.hpp"
#include "CompilerExceptions.hpp"

namespace Lust
{
	TEST(ShaderManager, IsSPVShaderCompilerWorkingForGraphicsPipeline)
	{
		Console::Init();
		SPVCompiler compiler("main", "_6_8", "1.3");
		compiler.SetBaseEntry("_main");
		FileHandler::WriteTextFile("SampleShader.hlsl", GraphicsFullShader);
		compiler.PushShaderPath("./SampleShader.hlsl", PipelineType::Graphics);
		compiler.CompilePackedShader();
		ASSERT_TRUE(FileHandler::FileExists("SampleShader.vk.json"));
		ASSERT_TRUE(FileHandler::FileExists("SampleShader.vs.spv"));
		ASSERT_TRUE(FileHandler::FileExists("SampleShader.ps.spv"));
		std::remove("SampleShader.hlsl");
		std::remove("SampleShader.vk.json");
		std::remove("SampleShader.vs.spv");
		std::remove("SampleShader.ps.spv");
		Console::End();
	}

	TEST(ShaderManager, InvalidVulkanVersionNumber)
	{
		Console::Init();
		std::shared_ptr<SPVCompiler> compiler;
		ASSERT_THROW(compiler.reset(new SPVCompiler("_main", "_6_8", "1.5")), InvalidVulkanVersion);
		Console::End();
	}

	TEST(ShaderManager, InvalidVulkanVersionFormat)
	{
		Console::Init();
		std::shared_ptr<SPVCompiler> compiler;
		compiler.reset(new SPVCompiler("_main", "_6_8", "1.3"));
		ASSERT_THROW(compiler->SetVulkanFeatureLevel("1_5"), InvalidVulkanVersion);
		Console::End();
	}

	TEST(ShaderManager, IsSPVShaderCompilerWorkingForGraphicsPipelineInRelease)
	{
		Console::Init();
		SPVCompiler compiler("main", "_6_8", "1.3");
		compiler.SetBaseEntry("_main");
		FileHandler::WriteTextFile("SampleShaderDefective.hlsl", GraphicsFullShaderMissingPixel);
		compiler.SetBuildMode(false);
		compiler.PushShaderPath("./SampleShaderDefective.hlsl", PipelineType::Graphics);
		ASSERT_THROW(compiler.CompilePackedShader(), InvalidPipelineException);
		std::remove("SampleShaderDefective.hlsl");
		Console::End();
	}
}
