#ifdef LUST_USES_WINDOWS
#include "D3D12Sampler.hpp"

Lust::D3D12Sampler::D3D12Sampler(const SamplerInfo& info)
{
	m_Info = info;
	m_Sampler.Filter = GetNativeFilter(info.GetFilter());
	m_Sampler.AddressU = GetNativeAddressMode(info.GetAddressMode());
	m_Sampler.AddressV = GetNativeAddressMode(info.GetAddressMode());
	m_Sampler.AddressW = GetNativeAddressMode(info.GetAddressMode());
	m_Sampler.MipLODBias = 0.0f;
	m_Sampler.MaxAnisotropy = 1 << (uint32_t)info.GetAnisotropicFactor();
	m_Sampler.ComparisonFunc = (D3D12_COMPARISON_FUNC)((uint32_t)info.GetComparisonPassMode() + 1);
	m_Sampler.MinLOD = 0.0f;
	m_Sampler.MaxLOD = D3D12_FLOAT32_MAX;
}

Lust::D3D12Sampler::~D3D12Sampler()
{
}

const D3D12_SAMPLER_DESC& Lust::D3D12Sampler::GetSampler() const
{
	return m_Sampler;
}

D3D12_FILTER Lust::D3D12Sampler::GetNativeFilter(SamplerFilter filter)
{
	switch (filter)
	{
	default:
	case SamplerFilter::LINEAR:
		return D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	case SamplerFilter::NEAREST:
		return D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
	case SamplerFilter::ANISOTROPIC:
		return D3D12_FILTER_ANISOTROPIC;
	}
}

D3D12_TEXTURE_ADDRESS_MODE Lust::D3D12Sampler::GetNativeAddressMode(AddressMode addressMode)
{
	switch (addressMode)
	{
	default:
	case AddressMode::REPEAT:
		return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	case AddressMode::MIRROR:
		return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
	case AddressMode::CLAMP:
		return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	case AddressMode::BORDER:
		return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	case AddressMode::MIRROR_ONCE:
		return D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE;
	}
}

#endif
