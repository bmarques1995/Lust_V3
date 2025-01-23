#include "ApplicationStarter.hpp"
#include "FileHandler.hpp"

const std::unordered_map<std::string, Lust::GraphicsAPI> Lust::ApplicationStarter::s_GraphicsAPIMapper =
{
	{"SAMPLE_RENDER_GRAPHICS_API_VK", GraphicsAPI::SAMPLE_RENDER_GRAPHICS_API_VK},
#ifdef LUST_USES_WINDOWS
	{"SAMPLE_RENDER_GRAPHICS_API_D3D12", GraphicsAPI::SAMPLE_RENDER_GRAPHICS_API_D3D12},
#endif
};

const std::unordered_map<Lust::GraphicsAPI, std::string> Lust::ApplicationStarter::s_GraphicsAPIReverseMapper =
{
	{GraphicsAPI::SAMPLE_RENDER_GRAPHICS_API_VK, "SAMPLE_RENDER_GRAPHICS_API_VK"},
#ifdef LUST_USES_WINDOWS
	{GraphicsAPI::SAMPLE_RENDER_GRAPHICS_API_D3D12, "SAMPLE_RENDER_GRAPHICS_API_D3D12"},
#endif
};


Lust::ApplicationStarter::ApplicationStarter(std::string_view jsonFilepath) :
	m_Filepath(jsonFilepath.data())
{
	if (!FileHandler::FileExists(jsonFilepath.data()))
	{
		m_Starter["GraphicsAPI"] = "SAMPLE_RENDER_GRAPHICS_API_VK";
		m_API = SAMPLE_RENDER_GRAPHICS_API_VK;
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

		auto it = s_GraphicsAPIMapper.find(m_Starter["GraphicsAPI"].asString());
		if ((it == s_GraphicsAPIMapper.end()) || (!healthy))
		{
			m_Starter["GraphicsAPI"] = "SAMPLE_RENDER_GRAPHICS_API_VK";
			m_API = SAMPLE_RENDER_GRAPHICS_API_VK;
			m_FullscreenMode = false;
			m_Starter["FullscreenMode"] = m_FullscreenMode;
			
			FileHandler::WriteTextFile(jsonFilepath.data(), m_Starter.toStyledString());
		}
		else
		{
			m_API = it->second;
			m_FullscreenMode = m_Starter["FullscreenMode"].asBool();
		}

	}
}

Lust::ApplicationStarter::~ApplicationStarter()
{
	FileHandler::WriteTextFile(m_Filepath, m_Starter.toStyledString());
}

bool Lust::ApplicationStarter::GetFullscreenMode()
{
	return m_FullscreenMode;
}

void Lust::ApplicationStarter::SetFullscreenMode(bool fullscreen)
{
	m_Starter["FullscreenMode"] = fullscreen;
	m_FullscreenMode = fullscreen;
}

Lust::GraphicsAPI Lust::ApplicationStarter::GetCurrentAPI()
{
	return m_API;
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
