#include "SPVCompiler.hpp"
#include "CompilerExceptions.hpp"
#include <regex>
#include <sstream>
#include <json/json.h>
#include "FileHandler.hpp"

const std::list<std::pair<uint32_t, uint32_t>> Lust::SPVCompiler::s_ValidVulkan =
{
	{1, 0},
	{1, 1},
	{1, 2},
	{1, 3},
	{1, 4}
};

Lust::SPVCompiler::SPVCompiler(std::string baseEntry, std::string hlslFeatureLevel, std::string vulkanFeatureLevel) :
	Lust::Compiler(".spv", ".vk", baseEntry, hlslFeatureLevel)
{
	SetVulkanFeatureLevel(vulkanFeatureLevel);
}

Lust::SPVCompiler::~SPVCompiler()
{
}

void Lust::SPVCompiler::SetVulkanFeatureLevel(std::string version)
{
	ValidateVulkanFeatureLevel(version);
	m_VulkanFeatureLevel = version;
	std::wstringstream vulkan_fl_buffer;
	vulkan_fl_buffer << L"-fspv-target-env=vulkan" << std::wstring(m_VulkanFeatureLevel.begin(), m_VulkanFeatureLevel.end());
	m_VulkanFeatureLevelArg = vulkan_fl_buffer.str();
}

void Lust::SPVCompiler::CompilePackedShader()
{
	static const std::regex pattern("^(.*[\\/])([^\\/]+)\\.hlsl$");
	std::smatch matches;

	Json::Value root;
	Json::StreamWriterBuilder builder;
	const std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
	uint32_t presentStages = 0;

	for (auto& shaderPath : m_ShaderFilepaths)
	{
		std::string basepath;
		if (std::regex_match(shaderPath.first, matches, pattern))
		{
			std::stringstream buffer;
			buffer << matches[1].str();
			buffer << matches[2].str();
			basepath = buffer.str();
			buffer.str("");
			std::string shader;
			ReadShaderSource(shaderPath.first, &shader);
			for (auto& stage : s_ShaderStages)
			{
				if ((stage.first.compare("lib") == 0) && (shaderPath.second != PipelineType::RayTracing))
					continue;
				PushArgList(stage.first);
				if (CompileStage(shader, stage.first, basepath))
				{
					presentStages |= (uint32_t)stage.second;
					buffer << matches[2].str() << "." << stage.first << m_BackendExtension;
					root["BinShaders"][stage.first]["filename"] = buffer.str();
					root["BinShaders"][stage.first]["entrypoint"] = m_CurrentEntrypointCopy;
					buffer.str("");
				}
				m_ArgList.clear();
			}
			if (!ValidatePipeline(presentStages, shaderPath.second))
				continue;
			root["HLSLFeatureLevel"] = m_HLSLFeatureLevel;
			root["VulkanFeatureLevel"] = m_VulkanFeatureLevel;
			writer->write(root, &buffer);
			std::string jsonResult = buffer.str();
			buffer.str("");
			buffer << basepath << m_GraphicsAPIExtension << ".json";
			std::string jsonPath = buffer.str();
			buffer.str("");
			FileHandler::WriteTextFile(jsonPath, jsonResult);
		}
		else
		{
			throw InvalidFilepathException("Invalid file extension, the only allowed is \".hlsl\"");
		}
	}
}

void Lust::SPVCompiler::PushArgList(std::string stage)
{
	std::stringstream buffer;
	std::string tempbuffer;

	buffer << stage << m_BaseEntry;
	tempbuffer = buffer.str();
	ValidateNameOverKeywords(tempbuffer);
	ValidateNameOverSysValues(tempbuffer);
	ValidateNameOverBuiltinFunctions(tempbuffer);
	m_CurrentEntrypointCopy = tempbuffer;
	m_CurrentEntrypoint = std::wstring(tempbuffer.begin(), tempbuffer.end());
	buffer.str("");
	buffer << stage << m_HLSLFeatureLevel;
	tempbuffer = buffer.str();
	m_CurrentFormattedStage = std::wstring(tempbuffer.begin(), tempbuffer.end());
	buffer.str("");

	m_ArgList.push_back(L"-Zpc");
	m_ArgList.push_back(L"-HV");
	m_ArgList.push_back(L"2021");
	m_ArgList.push_back(L"-T");
	m_ArgList.push_back(m_CurrentFormattedStage.c_str());
	m_ArgList.push_back(L"-E");
	m_ArgList.push_back(m_CurrentEntrypoint.c_str());
	if (m_DebugMode)
	{
		m_ArgList.push_back(L"-O0");
		m_ArgList.push_back(L"-fspv-debug=vulkan-with-source");
		m_ArgList.push_back(L"-fspv-extension=SPV_KHR_non_semantic_info");
	}
	else
		m_ArgList.push_back(L"-O3");

	m_ArgList.push_back(L"-spirv");
	m_ArgList.push_back(m_VulkanFeatureLevelArg.c_str());
	if ((stage.compare("vs") == 0) || (stage.compare("gs") == 0) || (stage.compare("ds") == 0))
		m_ArgList.push_back(L"-fvk-invert-y");
	m_ArgList.push_back(L"-D");
	m_ArgList.push_back(L"VK_HLSL");
}

void Lust::SPVCompiler::ValidateVulkanFeatureLevel(std::string version)
{
	std::regex pattern("^(\\d+)\\.(\\d+)$");
	std::smatch matches;
	uint32_t major = 0;
	uint32_t minor = 0;
	if (std::regex_search(version, matches, pattern)) {
		std::stringstream buffer;
		buffer << matches[1].str() << " " << matches[2].str();
		std::istringstream reader(buffer.str());
		reader >> major >> minor;
	}
	else
	{
		throw InvalidVulkanVersion("The HLSL version must match the pattern \"^(\\d+)\\.(\\d+)$\"");
	}
	auto it = SearchVulkanVersion(std::make_pair(major, minor));
	if (it == s_ValidVulkan.end())
	{
		std::stringstream valid_vulkan;
		valid_vulkan << "The valid Vulkan versions are: {\"";
		for (auto it = s_ValidVulkan.begin(); it != s_ValidVulkan.end(); it++)
			valid_vulkan << it->first << "." << it->second << "|";
		valid_vulkan << "\"}";
		throw InvalidVulkanVersion(valid_vulkan.str());
	}
}

std::list<std::pair<uint32_t, uint32_t>>::const_iterator Lust::SPVCompiler::SearchVulkanVersion(std::pair<uint32_t, uint32_t> value)
{
	for (auto it = s_ValidVulkan.begin(); it != s_ValidVulkan.end(); it++)
		if ((it->first == value.first) && (it->second == value.second))
			return it;
	return s_ValidVulkan.end();
}
