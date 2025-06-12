#include "GPUInstrumentator.hpp"
#include "Application.hpp"
#ifdef LUST_USES_WINDOWS
#include "D3D12Instrumentator.hpp"
#endif
#include "VKInstrumentator.hpp"

Lust::GPUInstrumentator* Lust::GPUInstrumentator::Instantiate(const GraphicsContext* context)
{
	GraphicsAPI api = Application::GetInstance()->GetCurrentAPI();
	switch (api)
	{
#ifdef LUST_USES_WINDOWS
	case Lust::RENDER_GRAPHICS_API_D3D12:
	{
		return new D3D12Instrumentator((const D3D12Context*)(context));
	}
#endif
	case Lust::RENDER_GRAPHICS_API_VK:
	{
		return new VKInstrumentator((const VKContext*)(context));
	}
	default:
		break;
	}
	return nullptr;
}
