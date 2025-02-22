#include "Buffer.hpp"
#include "Application.hpp"
#ifdef LUST_USES_WINDOWS
#include "D3D12Buffer.hpp"
#endif
#include "VKBuffer.hpp"

Lust::VertexBuffer* Lust::VertexBuffer::Instantiate(const GraphicsContext* context, const void* data, size_t size, uint32_t stride)
{
	GraphicsAPI api = Application::GetInstance()->GetCurrentAPI();
	switch (api)
	{
#ifdef LUST_USES_WINDOWS
	case Lust::SAMPLE_RENDER_GRAPHICS_API_D3D12:
	{
		return new D3D12VertexBuffer((const D3D12Context*)(context), data, size, stride);
	}
#endif
	case Lust::SAMPLE_RENDER_GRAPHICS_API_VK:
	{
		return new VKVertexBuffer((const VKContext*)(context), data, size, stride);
	}
	default:
		break;
	}
	return nullptr;
}

Lust::IndexBuffer* Lust::IndexBuffer::Instantiate(const GraphicsContext* context, const void* data, size_t count)
{
	GraphicsAPI api = Application::GetInstance()->GetCurrentAPI();
	switch (api)
	{
#ifdef LUST_USES_WINDOWS
	case Lust::SAMPLE_RENDER_GRAPHICS_API_D3D12:
	{
		return new D3D12IndexBuffer((const D3D12Context*)(context), data, count);
	}
#endif
	case Lust::SAMPLE_RENDER_GRAPHICS_API_VK:
	{
		return new VKIndexBuffer((const VKContext*)(context), data, count);
	}
	default:
		break;
	}
	return nullptr;
}

Lust::UniformBuffer* Lust::UniformBuffer::Instantiate(const GraphicsContext* context, const void* data, size_t size)
{
	GraphicsAPI api = Application::GetInstance()->GetCurrentAPI();
	switch (api)
	{
#ifdef LUST_USES_WINDOWS
	case Lust::SAMPLE_RENDER_GRAPHICS_API_D3D12:
	{
		return new D3D12UniformBuffer((const D3D12Context*)(context), data, size);
	}
#endif
	case Lust::SAMPLE_RENDER_GRAPHICS_API_VK:
	{
		return new VKUniformBuffer((const VKContext*)(context), data, size);
	}
	default:
		break;
	}
	return nullptr;
}

Lust::StructuredBuffer* Lust::StructuredBuffer::Instantiate(const GraphicsContext* context, const void* data, size_t size)
{
	GraphicsAPI api = Application::GetInstance()->GetCurrentAPI();
	switch (api)
	{
#ifdef LUST_USES_WINDOWS
	case Lust::SAMPLE_RENDER_GRAPHICS_API_D3D12:
	{
		return new D3D12StructuredBuffer((const D3D12Context*)(context), data, size);
	}
#endif
	case Lust::SAMPLE_RENDER_GRAPHICS_API_VK:
	{
		return new VKStructuredBuffer((const VKContext*)(context), data, size);
	}
	default:
		break;
	}
	return nullptr;
}
