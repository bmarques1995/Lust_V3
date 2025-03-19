#pragma once

#include "LustDLLMacro.hpp"
#include "GraphicsContext.hpp"
#include <memory>

namespace Lust
{
	/**
	* @brief Interface for GPU instrumentator
	* @details Handles GPU queries, such as GPU time
	*/
	class LUST_API GPUInstrumentator
	{
	public:
		virtual ~GPUInstrumentator() = default;

		/**
		* @brief Begin GPU time queries
		*/
		virtual void BeginQueryTime() = 0;
		/**
		* @brief End GPU time queries
		*/
		virtual void EndQueryTime() = 0;

		/**
		* @brief Get GPU time
		*/
		virtual double GetQueryTime() = 0;

		/**
		* @brief Instantiate GPU instrumentator, factory method
		* @param context Graphics context interface
		*/
		static GPUInstrumentator* Instantiate(const GraphicsContext* context);
	};
}