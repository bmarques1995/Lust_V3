#pragma once

#include "LustDLLMacro.hpp"
#include "GraphicsContext.hpp"
#include <SamplerLayout.hpp>

namespace Lust
{
	class LUST_API Sampler
	{
	public:
		virtual ~Sampler() = default;
	
		SamplerInfo GetInfo() const;

		static Sampler* Instantiate(const GraphicsContext* context, const SamplerInfo& info);

	protected:
		SamplerInfo m_Info;
	};
}