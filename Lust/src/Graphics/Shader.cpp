#include "Shader.hpp"
#include "Application.hpp"
#ifdef LUST_USES_WINDOWS
#include "D3D12Shader.hpp"
#include "D3D12Context.hpp"
#endif
#include "VKContext.hpp"
#include "VKShader.hpp"
#include "FileHandler.hpp"
#include <filesystem>

namespace fs = std::filesystem;

Lust::SizeMismatchException::SizeMismatchException(size_t layoutSize, size_t providedSize)
{
	std::stringstream buffer;
	buffer << "The expected size is: " << layoutSize << ", but the provided buffer size is: " << providedSize;
	m_Reason = buffer.str();
}

Lust::Shader::Shader(InputBufferLayout layout, SmallBufferLayout smallBufferLayout, UniformLayout uniformLayout, TextureLayout textureLayout, SamplerLayout samplerLayout)
	: m_Layout(layout), m_SmallBufferLayout(smallBufferLayout), m_UniformLayout(uniformLayout), m_TextureLayout(textureLayout), m_SamplerLayout(samplerLayout)
{
}

Lust::Shader* Lust::Shader::Instantiate(const std::shared_ptr<GraphicsContext>* context, std::string json_basepath, InputBufferLayout layout, SmallBufferLayout smallBufferLayout, UniformLayout uniformLayout, TextureLayout textureLayout, SamplerLayout samplerLayout)
{
	GraphicsAPI api = Application::GetInstance()->GetCurrentAPI();
	std::stringstream controller_path;
	controller_path << json_basepath;
	switch (api)
	{
#ifdef LUST_USES_WINDOWS
	case Lust::SAMPLE_RENDER_GRAPHICS_API_D3D12:
	{
		controller_path << ".d3d12.json";
		std::string json_controller_path = controller_path.str();
		return new D3D12Shader((const std::shared_ptr<D3D12Context>*)(context), json_controller_path, layout, smallBufferLayout, uniformLayout, textureLayout, samplerLayout);
	}
#endif
	case Lust::SAMPLE_RENDER_GRAPHICS_API_VK:
	{
		controller_path << ".vk.json";
		std::string json_controller_path = controller_path.str();
		return new VKShader((const std::shared_ptr<VKContext>*)(context), json_controller_path, layout, smallBufferLayout, uniformLayout, textureLayout, samplerLayout);
	}
	default:
		break;
	}
	return nullptr;
}

void Lust::Shader::InitJsonAndPaths(std::string json_controller_path, Json::Value* pipelineInfo, std::string* shaderDir)
{
	Json::Reader reader;
	std::string jsonResult;
	FileHandler::ReadTextFile(json_controller_path, &jsonResult);
	reader.parse(jsonResult, *pipelineInfo);

	fs::path location = json_controller_path;
	*shaderDir = location.parent_path().string();
}