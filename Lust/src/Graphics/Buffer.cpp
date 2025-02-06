#include "Buffer.hpp"
#include "Application.hpp"
#ifdef LUST_USES_WINDOWS
#include "D3D12Buffer.hpp"
#endif
#include "VKBuffer.hpp"

Lust::VertexBuffer* Lust::VertexBuffer::Instantiate(const std::shared_ptr<GraphicsContext>* context, const void* data, size_t size, uint32_t stride)
{
	GraphicsAPI api = Application::GetInstance()->GetCurrentAPI();
	switch (api)
	{
#ifdef LUST_USES_WINDOWS
	case Lust::SAMPLE_RENDER_GRAPHICS_API_D3D12:
	{
		return new D3D12VertexBuffer((const std::shared_ptr<D3D12Context>*)(context), data, size, stride);
	}
#endif
	case Lust::SAMPLE_RENDER_GRAPHICS_API_VK:
	{
		return new VKVertexBuffer((const std::shared_ptr<VKContext>*)(context), data, size, size);
	}
	default:
		break;
	}
	return nullptr;
}

Lust::IndexBuffer* Lust::IndexBuffer::Instantiate(const std::shared_ptr<GraphicsContext>* context, const void* data, size_t count)
{
	GraphicsAPI api = Application::GetInstance()->GetCurrentAPI();
	switch (api)
	{
#ifdef LUST_USES_WINDOWS
	case Lust::SAMPLE_RENDER_GRAPHICS_API_D3D12:
	{
		return new D3D12IndexBuffer((const std::shared_ptr<D3D12Context>*)(context), data, count);
	}
#endif
	case Lust::SAMPLE_RENDER_GRAPHICS_API_VK:
	{
		return new VKIndexBuffer((const std::shared_ptr<VKContext>*)(context), data, count);
	}
	default:
		break;
	}
	return nullptr;
}
