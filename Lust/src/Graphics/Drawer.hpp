#pragma once

#include "CopyPipeline.hpp"

namespace Lust
{
	/**
	* @brief Interface for a %drawer
	* @details Handles drawing operations
	*/
	class Drawer
	{
	public:
		/**
		* @brief Draw
		* @details Draw vertices, with optional instances
		* @param count Number of vertices
		* @param instances Number of instances
		*/
		virtual void Draw(uint32_t count, uint32_t instances = 1) = 0;
		/**
		* @brief DrawIndexed
		* @details Draw indexed vertices, with optional instances
		* @param count Number of indices
		* @param instances Number of instances
		*/
		virtual void DrawIndexed(uint32_t count, uint32_t instances = 1) = 0;

		/**
		* @brief Instantiate drawer, factory method
		* @param context Graphics context
		*/
		static Drawer* Instantiate(const GraphicsContext* context);
	};
}