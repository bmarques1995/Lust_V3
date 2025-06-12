#include "ShaderReflector.hpp"

#ifdef LUST_SHADER_MNG_USES_WINDOWS
#include "D3D12ShaderReflector.hpp"
#endif
#include "VKShaderReflector.hpp"

#include "FileHandler.hpp"
#include <filesystem>

namespace fs = std::filesystem;

uint32_t Lust::ShaderReflector::s_UniformAttachment;
uint32_t Lust::ShaderReflector::s_SmallBufferAttachment;
bool Lust::ShaderReflector::s_InitializedAttachments = false;
Lust::TargetAPI Lust::ShaderReflector::s_API;

const std::list<std::string> Lust::ShaderReflector::s_GraphicsPipelineStages =
{
	"vs",
	"ps",
	"gs",
	"ds",
	"hs",
};


Lust::ShaderReflector* Lust::ShaderReflector::Instantiate(std::string_view jsonBasepath, uint32_t stages, uint32_t numInstances)
{
	if (!s_InitializedAttachments)
	{
		return nullptr;
	}
	std::stringstream controller_path;
	controller_path << jsonBasepath.data();
	switch (s_API)
	{
#ifdef LUST_SHADER_MNG_USES_WINDOWS
	case Lust::SHADER_MANAGER_GRAPHICS_API_D3D12:
	{
		controller_path << ".d3d12.json";
		std::string json_controller_path = controller_path.str();
		return new D3D12ShaderReflector(json_controller_path, stages, numInstances);
	}
#endif
	case Lust::SHADER_MANAGER_GRAPHICS_API_VK:
	{
		controller_path << ".vk.json";
		std::string json_controller_path = controller_path.str();
		return new VKShaderReflector(json_controller_path, stages, numInstances);
	}
	default:
		break;
	}
	return nullptr;
}

void Lust::ShaderReflector::InitJsonAndPaths(std::string_view jsonFilepath)
{
	Json::Reader reader;
	std::string jsonResult;
	FileHandler::ReadTextFile(jsonFilepath, &jsonResult);
	reader.parse(jsonResult, m_PipelineInfo);

	fs::path location = jsonFilepath;
	m_ShaderDir = location.parent_path().string();
}

Lust::ShaderReflector::ShaderReflector(uint32_t stages, uint32_t numInstances) :
	m_SmallBufferLayout({ {}, stages }), m_UniformLayout({ {}, stages }), 
	m_StructuredBufferLayout({ {}, stages }), m_NumberOfInstances(numInstances),
	m_TextureLayout({ {}, stages }), m_SamplerLayout({ {} }),
	m_TextureArrayLayout({ {}, stages }), m_SamplerArrayLayout({ {} })
{
}

const Lust::InputBufferLayout& Lust::ShaderReflector::GetInputLayout() const
{
	return m_InputBufferLayout;
}

const Lust::SmallBufferLayout& Lust::ShaderReflector::GetSmallBufferLayout() const
{
	return m_SmallBufferLayout;
}

const Lust::UniformLayout& Lust::ShaderReflector::GetUniformLayout() const
{
	return m_UniformLayout;
}

const Lust::TextureLayout& Lust::ShaderReflector::GetTextureLayout() const
{
	return m_TextureLayout;
}

const Lust::SamplerLayout& Lust::ShaderReflector::GetSamplerLayout() const
{
	return m_SamplerLayout;
}

const Lust::TextureArrayLayout& Lust::ShaderReflector::GetTextureArrayLayout() const
{
	return m_TextureArrayLayout;
}

const Lust::SamplerArrayLayout& Lust::ShaderReflector::GetSamplerArrayLayout() const
{
	return m_SamplerArrayLayout;
}

const Lust::StructuredBufferLayout& Lust::ShaderReflector::GetStructuredBufferLayout() const
{
	return m_StructuredBufferLayout;
}

uint32_t Lust::ShaderReflector::GetUniformAttachment()
{
	return s_UniformAttachment;
}

uint32_t Lust::ShaderReflector::GetSmallBufferAttachment()
{
	return s_SmallBufferAttachment;
}

void Lust::ShaderReflector::InitAPIInfos(uint32_t uniformAttachment, uint32_t smallBufferAttachment, TargetAPI api)
{
	if (!s_InitializedAttachments)
	{
		s_API = api;
		s_SmallBufferAttachment = smallBufferAttachment;
		s_UniformAttachment = uniformAttachment;
		s_InitializedAttachments = true;
	}
}
