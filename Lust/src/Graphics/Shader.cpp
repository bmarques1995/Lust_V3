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

Lust::InputInfo::InputInfo(uint32_t stages) :
	m_SmallBufferLayout({ {}, stages }), m_UniformLayout({ {}, stages }),
	m_StructuredBufferLayout({ {}, stages }), m_InputLayout({ {} }),
	m_TextureLayout({ {}, stages }), m_SamplerLayout({ {} }),
	m_TextureArrayLayout({ {}, stages }), m_SamplerArrayLayout({ {} })
{
}

Lust::InputInfo::InputInfo(InputBufferLayout inputLayout, SmallBufferLayout smallBufferLayout, UniformLayout uniformLayout, TextureLayout textureLayout, SamplerLayout samplerLayout,
	TextureArrayLayout textureArrayLayout, SamplerArrayLayout samplerArrayLayout, StructuredBufferLayout structuredBufferLayout) :
	m_InputLayout(inputLayout),
	m_SmallBufferLayout(smallBufferLayout),
	m_UniformLayout(uniformLayout),
	m_TextureLayout(textureLayout),
	m_SamplerLayout(samplerLayout),
	m_TextureArrayLayout(textureArrayLayout),
	m_SamplerArrayLayout(samplerArrayLayout),
	m_StructuredBufferLayout(structuredBufferLayout)
{
}

Lust::Shader::Shader(const std::shared_ptr<ShaderReflector>& inputInfo, const std::string& filepath)
	: m_ShaderReflector(inputInfo)
{
	std::filesystem::path fpath = filepath;
	SetName(fpath.stem().stem().string());
}

const Lust::InputBufferLayout& Lust::Shader::GetInputLayout() const
{
	return m_ShaderReflector->GetInputLayout();
}

const Lust::SmallBufferLayout& Lust::Shader::GetSmallBufferLayout() const
{
	return m_ShaderReflector->GetSmallBufferLayout();
}

const Lust::UniformLayout& Lust::Shader::GetUniformLayout() const
{
	return m_ShaderReflector->GetUniformLayout();
}

const Lust::StructuredBufferLayout& Lust::Shader::GetStructuredBufferLayout() const
{
	return m_ShaderReflector->GetStructuredBufferLayout();
}

const Lust::TextureLayout& Lust::Shader::GetTextureLayout() const
{
	return m_ShaderReflector->GetTextureLayout();
}

const Lust::SamplerLayout& Lust::Shader::GetSamplerLayout() const
{
	return m_ShaderReflector->GetSamplerLayout();
}

const Lust::TextureArrayLayout& Lust::Shader::GetTextureArrayLayout() const
{
	return m_ShaderReflector->GetTextureArrayLayout();
}

const Lust::SamplerArrayLayout& Lust::Shader::GetSamplerArrayLayout() const
{
	return m_ShaderReflector->GetSamplerArrayLayout();
}

const std::string& Lust::Shader::GetName() const
{
	return m_Name;
}

void Lust::Shader::SetName(const std::string& name)
{
	m_Name = name;
}

Lust::Shader* Lust::Shader::Instantiate(const GraphicsContext* context, std::string json_basepath, const std::shared_ptr<ShaderReflector>& inputInfo, const Topology& topology)
{
	GraphicsAPI api = Application::GetInstance()->GetCurrentAPI();
	std::stringstream controller_path;
	controller_path << json_basepath;
	switch (api)
	{
#ifdef LUST_USES_WINDOWS
	case Lust::RENDER_GRAPHICS_API_D3D12:
	{
		controller_path << ".d3d12.json";
		std::string json_controller_path = controller_path.str();
		return new D3D12Shader((const D3D12Context*)(context), json_controller_path, inputInfo, topology);
	}
#endif
	case Lust::RENDER_GRAPHICS_API_VK:
	{
		controller_path << ".vk.json";
		std::string json_controller_path = controller_path.str();
		return new VKShader((const VKContext*)(context), json_controller_path, inputInfo, topology);
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

Lust::ShaderLibrary::ShaderLibrary(const GraphicsContext* context) :
	m_Context(context)
{

}

Lust::ShaderLibrary::~ShaderLibrary()
{
	m_Shaders.clear();
	m_Context = nullptr;
}

void Lust::ShaderLibrary::Add(const std::string& name, const std::shared_ptr<Shader>& shader)
{
	Console::CoreAssert(!Exists(name), "Shader already exists!");
	m_Shaders[name] = shader;
}

void Lust::ShaderLibrary::Add(const std::shared_ptr<Shader>& shader)
{
	auto& name = shader->GetName();
	Add(name, shader);
}

std::shared_ptr<Lust::Shader> Lust::ShaderLibrary::Load(const std::string& json_basepath)
{
	std::shared_ptr<ShaderReflector> inputInfo;
	inputInfo.reset(Lust::ShaderReflector::Instantiate(json_basepath, Lust::AllowedStages::VERTEX_STAGE | Lust::AllowedStages::PIXEL_STAGE));
	std::shared_ptr<Shader> shader;
	shader.reset(Shader::Instantiate(m_Context, json_basepath, inputInfo));
	Add(shader);
	return shader;
}

std::shared_ptr<Lust::Shader> Lust::ShaderLibrary::Load(const std::string& name, const std::string& json_basepath)
{
	std::shared_ptr<ShaderReflector> inputInfo;
	inputInfo.reset(Lust::ShaderReflector::Instantiate(json_basepath, Lust::AllowedStages::VERTEX_STAGE | Lust::AllowedStages::PIXEL_STAGE));
	std::shared_ptr<Shader> shader;
	shader.reset(Shader::Instantiate(m_Context, json_basepath, inputInfo));
	Add(name, shader);
	return shader;
}

std::shared_ptr<Lust::Shader> Lust::ShaderLibrary::Get(const std::string& name)
{
	Console::CoreAssert(Exists(name), "Shader not found!");
	return m_Shaders[name];
}

bool Lust::ShaderLibrary::Exists(const std::string& name) const
{
	return m_Shaders.find(name) != m_Shaders.end();
}