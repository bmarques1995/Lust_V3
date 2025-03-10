#pragma once 

#ifdef LUST_USES_WINDOWS

#include "LustDLLMacro.hpp"
#include "D3D12Context.hpp"
#include "Sampler.hpp"

namespace Lust
{
	class LUST_API D3D12Sampler : public Sampler
	{
	public:
		D3D12Sampler(const SamplerInfo& info);
		~D3D12Sampler();

		const D3D12_SAMPLER_DESC& GetSampler() const;
	
	private:
		static D3D12_FILTER GetNativeFilter(SamplerFilter filter);
		static D3D12_TEXTURE_ADDRESS_MODE GetNativeAddressMode(AddressMode addressMode);

		D3D12_SAMPLER_DESC m_Sampler;
	};
}

#endif