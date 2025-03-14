#pragma once

#include "LustDLLMacro.hpp"
#include <json/json.h>
#include "GraphicsContext.hpp"
#include <unordered_map>

namespace Lust
{
	class LUST_API ApplicationStarter
	{
	public:
		ApplicationStarter(std::string_view jsonFilepath);
		~ApplicationStarter();

		bool GetFullscreenMode() const;

		void SetFullscreenMode(bool fullscreen);

		GraphicsAPI GetCurrentAPI() const;
		uint32_t GetWidth() const;
		uint32_t GetHeight() const;

		void SetAPI(GraphicsAPI api);
	private:
		Json::Value m_Starter;
		std::string m_Filepath;
		
		GraphicsAPI m_API;
		bool m_FullscreenMode;
		uint32_t m_Width;
		uint32_t m_Height;

		static const std::unordered_map<std::string, GraphicsAPI> s_GraphicsAPIMapper;
		static const std::unordered_map<GraphicsAPI, std::string> s_GraphicsAPIReverseMapper;

		bool IsPropertyPresent(std::string_view property);
	};
}