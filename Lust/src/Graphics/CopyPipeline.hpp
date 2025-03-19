#pragma once
#include "LustDLLMacro.hpp"
#include "GraphicsContext.hpp"
#include <memory>

namespace Lust
{
	/**
	* @brief Represents a set that can handle copy operations
	* @details One of the key operations is to copy a texture or other resources for their final resources,
	* that are unaccessible from the CPU
	*/
	class LUST_API CopyPipeline
	{
	public:
		/**
		* @brief Copy pipeline destructor
		*/
		virtual ~CopyPipeline() = default;

		/**
		* @brief Wait for the copy pipeline to finish
		*/
		virtual void Wait() = 0;

		/**
		* @brief Copy pipeline factory
		* @param context Graphics context interface
		*/
		static CopyPipeline* Instantiate(const GraphicsContext* context);
	};
}