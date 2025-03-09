#include "Controller.hpp"
#include "ExampleLayer.hpp"
#include <CompilerExceptions.hpp>
#include <Console.hpp>
#include <Compiler.hpp>

Controller::Controller()
{
	try
	{
		m_SPVCompiler->PushShaderPath("./Examples/Layers/assets/shaders/HelloTriangle.hlsl", Lust::PipelineType::Graphics);
		m_CSOCompiler->PushShaderPath("./Examples/Layers/assets/shaders/HelloTriangle.hlsl", Lust::PipelineType::Graphics);
		m_SPVCompiler->PushShaderPath("./Examples/Layers/assets/shaders/FlatColor.hlsl", Lust::PipelineType::Graphics);
		m_CSOCompiler->PushShaderPath("./Examples/Layers/assets/shaders/FlatColor.hlsl", Lust::PipelineType::Graphics);
		m_SPVCompiler->CompilePackedShader();
		m_CSOCompiler->CompilePackedShader();
	}
	catch (Lust::CompilerException e)
	{
		Lust::Console::Error("{}", e.what());
	}

	PushLayer(new ExampleLayer());
}

Controller::~Controller()
{
}
