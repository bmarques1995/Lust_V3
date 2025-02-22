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

Lust::InputInfo::InputInfo(InputBufferLayout inputLayout, SmallBufferLayout smallBufferLayout, UniformLayout uniformLayout, TextureLayout textureLayout, SamplerLayout samplerLayout, StructuredBufferLayout structuredBufferLayout) :
	m_InputLayout(inputLayout),
	m_SmallBufferLayout(smallBufferLayout),
	m_UniformLayout(uniformLayout),
	m_TextureLayout(textureLayout),
	m_SamplerLayout(samplerLayout),
	m_StructuredBufferLayout(structuredBufferLayout)
{
}

Lust::Shader::Shader(const InputInfo& inputInfo)
	: m_Layout(inputInfo.m_InputLayout),
	m_SmallBufferLayout(inputInfo.m_SmallBufferLayout),
	m_UniformLayout(inputInfo.m_UniformLayout),
	m_TextureLayout(inputInfo.m_TextureLayout),
	m_SamplerLayout(inputInfo.m_SamplerLayout),
	m_StructuredBufferLayout(inputInfo.m_StructuredBufferLayout)
{
}

const Lust::InputBufferLayout& Lust::Shader::GetInputLayout() const
{
	return m_Layout;
}

const Lust::SmallBufferLayout& Lust::Shader::GetSmallBufferLayout() const
{
	return m_SmallBufferLayout;
}

const Lust::UniformLayout& Lust::Shader::GetUniformLayout() const
{
	return m_UniformLayout;
}

const Lust::TextureLayout& Lust::Shader::GetTextureLayout() const
{
	return m_TextureLayout;
}

const Lust::SamplerLayout& Lust::Shader::GetSamplerLayout() const
{
	return m_SamplerLayout;
}

const std::unordered_map<uint64_t, Lust::TextureElement>& Lust::Shader::GetTextureElements() const
{
	return m_TextureLayout.GetElements();
}

const std::unordered_map<uint32_t, Lust::UniformElement>& Lust::Shader::GetUniformElements() const
{
	return m_UniformLayout.GetElements();
}

const std::unordered_map<uint64_t, Lust::StructuredBufferElement>& Lust::Shader::GetStructuredBufferElements() const
{
	return m_StructuredBufferLayout.GetElements();
}

Lust::Shader* Lust::Shader::Instantiate(const GraphicsContext* context, std::string json_basepath, const InputInfo& inputInfo)
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
		return new D3D12Shader((const D3D12Context*)(context), json_controller_path, inputInfo);
	}
#endif
	case Lust::SAMPLE_RENDER_GRAPHICS_API_VK:
	{
		controller_path << ".vk.json";
		std::string json_controller_path = controller_path.str();
		return new VKShader((const VKContext*)(context), json_controller_path, inputInfo);
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