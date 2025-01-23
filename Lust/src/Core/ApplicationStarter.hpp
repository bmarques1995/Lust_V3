#pragma once

#include <json/json.h>
//#include "GraphicsContext.hpp"
#include <unordered_map>

namespace Lust
{
	class LUST_API ApplicationStarter
	{
	public:
		ApplicationStarter(std::string_view jsonFilepath);
		~ApplicationStarter();

		bool GetFullscreenMode();

		void SetFullscreenMode(bool fullscreen);

		//GraphicsAPI GetCurrentAPI();
	private:
		Json::Value m_Starter;
		std::string m_Filepath;
		//GraphicsAPI m_API;

		bool m_FullscreenMode;
		//static const std::unordered_map<std::string, GraphicsAPI> s_GraphicsAPIMapper;

		bool IsPropertyPresent(std::string_view property);
	};
}