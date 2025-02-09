#include "Drawer.hpp"
#include "Application.hpp"
#ifdef LUST_USES_WINDOWS
#include "D3D12Drawer.hpp"
#endif
#include "VKDrawer.hpp"

Lust::Drawer* Lust::Drawer::Instantiate(const GraphicsContext* context)
{
	GraphicsAPI api = Application::GetInstance()->GetCurrentAPI();
	switch (api)
	{
#ifdef LUST_USES_WINDOWS
	case SAMPLE_RENDER_GRAPHICS_API_D3D12:
	{
		return new D3D12Drawer((const D3D12Context*)(context));
	}
#endif
	case SAMPLE_RENDER_GRAPHICS_API_VK:
	{
		return new VKDrawer((const VKContext*)(context));
	}
	default:
		break;
	}
	return nullptr;
}
