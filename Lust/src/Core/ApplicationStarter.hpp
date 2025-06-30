#pragma once

#include "LustDLLMacro.hpp"
#include <json/json.h>
#include "GraphicsContext.hpp"
#include "Languages.hpp"
#include <unordered_map>

namespace Lust
{
	/**
	* @brief %ApplicationStarter class, handles startup configuration for the application, with a json file
	* @details elements handled by the json file are: ["GraphicsAPI"], ["FullscreenMode"], window ["Width"] and ["Height"]
	* with GraphicsAPI being mapped with these values: <br>
	* {"SAMPLE_RENDER_GRAPHICS_API_VK", Lust::GraphicsAPI::SAMPLE_RENDER_GRAPHICS_API_VK}, <br>
	* {"SAMPLE_RENDER_GRAPHICS_API_VULKAN", Lust::GraphicsAPI::SAMPLE_RENDER_GRAPHICS_API_VK}, <br>
	* {"SAMPLE_RENDER_GRAPHICS_API_DX12", Lust::GraphicsAPI::SAMPLE_RENDER_GRAPHICS_API_D3D12}, <br>
	* {"SAMPLE_RENDER_GRAPHICS_API_D3D12", Lust::GraphicsAPI::SAMPLE_RENDER_GRAPHICS_API_D3D12}, <br>
	* {"SAMPLE_RENDER_GRAPHICS_API_DIRECTX12", Lust::GraphicsAPI::SAMPLE_RENDER_GRAPHICS_API_D3D12}, <br>
	* {"SAMPLE_RENDER_GRAPHICS_API_DIRECT3D12", Lust::GraphicsAPI::SAMPLE_RENDER_GRAPHICS_API_D3D12}
	*/
	class LUST_API ApplicationStarter
	{
	public:
		/**
		* @brief ApplicationStarter constructor
		* @details constructor for ApplicationStarter class
		* @param jsonFilepath json file path
		*/
		ApplicationStarter(std::string_view jsonFilepath);
		/**
		* @brief ApplicationStarter destructor
		*/
		~ApplicationStarter();

		/**
		* @brief GetFullscreenMode
		* @details returns fullscreen mode
		*/
		bool GetFullscreenMode() const;

		/**
		* @brief SetFullscreenMode
		* @details sets fullscreen mode
		*/
		void SetFullscreenMode(bool fullscreen);

		/**
		* @brief GetAPI
		* @details returns GraphicsAPI
		*/
		GraphicsAPI GetCurrentAPI() const;
		/**
		* @brief GetWidth
		* @details returns width
		*/
		uint32_t GetWidth() const;
		/**
		* @brief GetHeight
		* @details returns height
		*/
		uint32_t GetHeight() const;

		/**
		* @brief GetCurrentLanguage
		* @details returns current language
		*/
		Language GetCurrentLanguage() const;

		/**
		* @brief SetAPI
		* @details sets GraphicsAPI
		*/
		void SetAPI(GraphicsAPI api);
	private:
		Json::Value m_Starter;
		std::string m_Filepath;
		
		GraphicsAPI m_API;
		bool m_FullscreenMode;
		uint32_t m_Width;
		uint32_t m_Height;
		Language m_Language;

		static const std::unordered_map<std::string, GraphicsAPI> s_GraphicsAPIMapper;
		static const std::unordered_map<GraphicsAPI, std::string> s_GraphicsAPIReverseMapper;
		

		/**
		* @brief IsPropertyPresent
		* @details checks if a property is present
		*/
		bool IsPropertyPresent(std::string_view property);
	};
}