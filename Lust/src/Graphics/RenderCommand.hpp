#pragma once

#include "Drawer.hpp"
#include "GraphicsContext.hpp"

namespace Lust {

	/**
	* @brief Render command
	* @details Handles drawing operations
	*/
	class LUST_API RenderCommand
	{
	public:
		/**
		* @brief Render command initializer
		*/
		static void Init(const GraphicsContext* context)
		{
			s_Drawer.reset(Drawer::Instantiate(context));
		}
		
		/**
		* @brief Render command shutdown
		*/
		static void Shutdown()
		{
			s_Drawer.reset();
		}

		/**
		* @brief Render command draw
		* @param elementCount Number of vertices
		* @param instanceCount Number of instances
		* @details Draws vertices
		*/
		static void Draw(uint32_t elementCount, uint32_t instanceCount = 1)
		{
			s_Drawer->Draw(elementCount, instanceCount);
		}

		/**
		* @brief Render command draw indexed
		* @param indexCount Number of indices
		* @param instanceCount Number of instances
		* @details Draws indexed vertices
		*/
		static void DrawIndexed(uint32_t indexCount, uint32_t instanceCount = 1)
		{
			s_Drawer->DrawIndexed(indexCount, instanceCount);
		}
		
	private:
		static std::unique_ptr<Drawer> s_Drawer;
	};

}