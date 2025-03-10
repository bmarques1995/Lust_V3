#pragma once

#include "LustDLLMacro.hpp"
#include "VKContext.hpp"
#include "Sampler.hpp"

namespace Lust
{
	class LUST_API VKSampler : public Sampler
	{
	public:
		VKSampler(const VKContext* context, const SamplerInfo& info);
		~VKSampler();

		VkSampler GetSampler() const;
	private:
		static VkFilter GetNativeFilter(SamplerFilter filter);
		static VkSamplerAddressMode GetNativeAddressMode(AddressMode addressMode);

		const VKContext* m_Context;
		VkSampler m_Sampler;
	};
}