#include "CopyPipeline.hpp"
#include "Application.hpp"
#ifdef LUST_USES_WINDOWS
#include "D3D12CopyPipeline.hpp"
#endif
#include "VKCopyPipeline.hpp"

Lust::CopyPipeline* Lust::CopyPipeline::Instantiate(const GraphicsContext* context)
{
	GraphicsAPI api = Application::GetInstance()->GetCurrentAPI();
	switch (api)
	{
#ifdef LUST_USES_WINDOWS
	case Lust::RENDER_GRAPHICS_API_D3D12:
	{
		return new D3D12CopyPipeline((const D3D12Context*)(context));
	}
#endif
	case Lust::RENDER_GRAPHICS_API_VK:
	{
		return new VKCopyPipeline((const VKContext*)(context));
	}
	default:
		break;
	}
	return nullptr;
}
