#include "ApplicationStarter.hpp"
#include "FileHandler.hpp"

const std::unordered_map<std::string, Lust::GraphicsAPI> Lust::ApplicationStarter::s_GraphicsAPIMapper =
{
	{"SAMPLE_RENDER_GRAPHICS_API_VK", Lust::GraphicsAPI::RENDER_GRAPHICS_API_VK},
	{"SAMPLE_RENDER_GRAPHICS_API_VULKAN", Lust::GraphicsAPI::RENDER_GRAPHICS_API_VK},
#ifdef LUST_USES_WINDOWS
	{"SAMPLE_RENDER_GRAPHICS_API_DX12", Lust::GraphicsAPI::RENDER_GRAPHICS_API_D3D12},
	{"SAMPLE_RENDER_GRAPHICS_API_D3D12", Lust::GraphicsAPI::RENDER_GRAPHICS_API_D3D12},
	{"SAMPLE_RENDER_GRAPHICS_API_DIRECTX12", Lust::GraphicsAPI::RENDER_GRAPHICS_API_D3D12},
	{"SAMPLE_RENDER_GRAPHICS_API_DIRECT3D12", Lust::GraphicsAPI::RENDER_GRAPHICS_API_D3D12},
#endif
};

const std::unordered_map<Lust::GraphicsAPI, std::string> Lust::ApplicationStarter::s_GraphicsAPIReverseMapper =
{
	{GraphicsAPI::RENDER_GRAPHICS_API_VK, "SAMPLE_RENDER_GRAPHICS_API_VK"},
#ifdef LUST_USES_WINDOWS
	{GraphicsAPI::RENDER_GRAPHICS_API_D3D12, "SAMPLE_RENDER_GRAPHICS_API_D3D12"},
#endif
};

Lust::ApplicationStarter::ApplicationStarter(std::string_view jsonFilepath) :
	m_Filepath(jsonFilepath.data())
{
	if (!FileHandler::FileExists(jsonFilepath.data()))
	{
		m_Starter["GraphicsAPI"] = "SAMPLE_RENDER_GRAPHICS_API_VK";
		m_API = RENDER_GRAPHICS_API_VK;
		FileHandler::WriteTextFile(jsonFilepath.data(), m_Starter.toStyledString());
		m_Starter["FullscreenMode"] = false;
		m_FullscreenMode = false;
	}
	else
	{
		Json::Reader reader;
		std::string jsonResult;
		bool healthy = FileHandler::ReadTextFile(jsonFilepath.data(), &jsonResult);
		healthy &= reader.parse(jsonResult, m_Starter);
		healthy &= IsPropertyPresent("FullscreenMode");
		healthy &= IsPropertyPresent("GraphicsAPI");
		healthy &= IsPropertyPresent("Width");
		healthy &= IsPropertyPresent("Height");
		healthy &= IsPropertyPresent("Language");

		auto it = s_GraphicsAPIMapper.find(m_Starter["GraphicsAPI"].asString());
		auto langIt = LanguageDefinitions::s_LanguageMapper.find(m_Starter["Language"].asString());
		if ((it == s_GraphicsAPIMapper.end()) || (langIt == LanguageDefinitions::s_LanguageMapper.end()) || (!healthy))
		{
			m_Width = 1280;
			m_Height = 720;
			m_API = RENDER_GRAPHICS_API_VK;
			m_FullscreenMode = false;
			m_Language = Language::EN_US;
			m_Starter["GraphicsAPI"] = "SAMPLE_RENDER_GRAPHICS_API_VK";
			m_Starter["FullscreenMode"] = m_FullscreenMode;
			m_Starter["Width"] = m_Width;
			m_Starter["Height"] = m_Height;
			m_Starter["Language"] = "EN_US";

			FileHandler::WriteTextFile(jsonFilepath.data(), m_Starter.toStyledString());
		}
		else
		{
			m_API = it->second;
			m_FullscreenMode = m_Starter["FullscreenMode"].asBool();
			m_Width = m_Starter["Width"].as<uint32_t>();
			m_Height = m_Starter["Height"].as<uint32_t>();
			m_Language = langIt->second;
		}

	}
}

Lust::ApplicationStarter::~ApplicationStarter()
{
	FileHandler::WriteTextFile(m_Filepath, m_Starter.toStyledString());
}

bool Lust::ApplicationStarter::GetFullscreenMode() const
{
	return m_FullscreenMode;
}

void Lust::ApplicationStarter::SetFullscreenMode(bool fullscreen)
{
	m_Starter["FullscreenMode"] = fullscreen;
	m_FullscreenMode = fullscreen;
}

Lust::GraphicsAPI Lust::ApplicationStarter::GetCurrentAPI() const
{
	return m_API;
}

uint32_t Lust::ApplicationStarter::GetWidth() const
{
	return m_Width;
}

uint32_t Lust::ApplicationStarter::GetHeight() const
{
	return m_Height;
}

Lust::Language Lust::ApplicationStarter::GetCurrentLanguage() const
{
	return m_Language;
}

void Lust::ApplicationStarter::SetAPI(GraphicsAPI api)
{
	m_API = api;
	auto it = s_GraphicsAPIReverseMapper.find(api);
	if (it != s_GraphicsAPIReverseMapper.end())
		m_Starter["GraphicsAPI"] = it->second;
}

bool Lust::ApplicationStarter::IsPropertyPresent(std::string_view property)
{
	return m_Starter.isMember(property.data());
}
