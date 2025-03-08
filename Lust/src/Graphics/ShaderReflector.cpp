#include "ShaderReflector.hpp"
#include "Application.hpp"
#ifdef LUST_USES_WINDOWS
#include "D3D12ShaderReflector.hpp"
#endif
#include "VKShaderReflector.hpp"

#include "FileHandler.hpp"
#include <filesystem>

namespace fs = std::filesystem;

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
	GraphicsAPI api = Application::GetInstance()->GetCurrentAPI();
	std::stringstream controller_path;
	controller_path << jsonBasepath.data();
	switch (api)
	{
#ifdef LUST_USES_WINDOWS
	case Lust::SAMPLE_RENDER_GRAPHICS_API_D3D12:
	{
		controller_path << ".d3d12.json";
		std::string json_controller_path = controller_path.str();
		return new D3D12ShaderReflector(json_controller_path, stages, numInstances);
	}
#endif
	case Lust::SAMPLE_RENDER_GRAPHICS_API_VK:
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
	m_TextureLayout({ {}, stages }), m_SamplerLayout({ {} })
{
}

void Lust::ShaderReflector::ExportInputLayout(InputBufferLayout* inputBufferLayout)
{
	Console::CoreLog("inspect time");
}
