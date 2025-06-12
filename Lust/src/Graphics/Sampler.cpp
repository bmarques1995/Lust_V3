#include "Sampler.hpp"
#include "Application.hpp"
#ifdef LUST_USES_WINDOWS
#include "D3D12Sampler.hpp"
#endif
#include "VKContext.hpp"
#include "VKSampler.hpp"

Lust::SamplerInfo Lust::Sampler::GetInfo() const
{
    return m_Info;
}

Lust::Sampler* Lust::Sampler::Instantiate(const GraphicsContext* context, const SamplerInfo& info)
{
	GraphicsAPI api = Application::GetInstance()->GetCurrentAPI();
	switch (api)
	{
#ifdef LUST_USES_WINDOWS
	case Lust::RENDER_GRAPHICS_API_D3D12:
	{
		return new D3D12Sampler(info);
	}
#endif
	case Lust::RENDER_GRAPHICS_API_VK:
	{
		return new VKSampler((const VKContext*)(context), info);
	}
	default:
		break;
	}
    return nullptr;
}
