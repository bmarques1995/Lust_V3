#include "ApplicationStarter.hpp"
#include "FileHandler.hpp"

Lust::ApplicationStarter::ApplicationStarter(std::string_view jsonFilepath) :
	m_Filepath(jsonFilepath.data())
{
	if (!FileHandler::FileExists(jsonFilepath.data()))
	{
		//m_Starter["GraphicsAPI"] = "SAMPLE_RENDER_GRAPHICS_API_VK";
		//m_API = SAMPLE_RENDER_GRAPHICS_API_VK;
		FileHandler::WriteTextFile(jsonFilepath.data(), m_Starter.toStyledString());
		m_Starter["FullscreenMode"] = false;
	}
	else
	{
		Json::Reader reader;
		std::string jsonResult;
		bool healthy = FileHandler::ReadTextFile(jsonFilepath.data(), &jsonResult);
		healthy &= reader.parse(jsonResult, m_Starter);
		healthy &= IsPropertyPresent("FullscreenMode");

		//auto it = s_GraphicsAPIMapper.find(m_Starter["GraphicsAPI"].asString());
		if (/*(it == s_GraphicsAPIMapper.end()) && */(!healthy))
		{
			//m_Starter["GraphicsAPI"] = "SAMPLE_RENDER_GRAPHICS_API_VK";
			//m_API = SAMPLE_RENDER_GRAPHICS_API_VK;
			m_FullscreenMode = false;
			m_Starter["FullscreenMode"] = m_FullscreenMode;
			
			FileHandler::WriteTextFile(jsonFilepath.data(), m_Starter.toStyledString());
		}
		else
		{
			//m_API = it->second;
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

bool Lust::ApplicationStarter::IsPropertyPresent(std::string_view property)
{
	return m_Starter.isMember(property.data());
}
