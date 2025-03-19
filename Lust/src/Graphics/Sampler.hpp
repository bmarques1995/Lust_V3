#pragma once

#include "LustDLLMacro.hpp"
#include "GraphicsContext.hpp"
#include <SamplerLayout.hpp>

namespace Lust
{
	/**
	* @brief Interface for a sampler
	*/
	class LUST_API Sampler
	{
	public:
		/**
		* @brief Sampler destructor
		*/
		virtual ~Sampler() = default;
	
		/**
		* @brief Get sampler info, see SamplerInfo
		*/
		SamplerInfo GetInfo() const;

		/**
		* @brief Instantiate sampler
		* @param context Graphics context interface
		* @param info Sampler info
		*/
		static Sampler* Instantiate(const GraphicsContext* context, const SamplerInfo& info);

	protected:
		SamplerInfo m_Info;
	};
}