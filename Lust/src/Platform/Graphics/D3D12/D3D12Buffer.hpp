#pragma once

#ifdef LUST_USES_WINDOWS

#include "D3D12Context.hpp"
#include "Buffer.hpp"
#include <memory>

namespace Lust
{
	class LUST_API D3D12Buffer
	{
	protected:
		D3D12Buffer(const D3D12Context* context);
		void CreateBuffer(const void* data, size_t size);
		void DestroyBuffer();

		ComPointer<ID3D12Resource2> m_Buffer;
		const D3D12Context* m_Context;
	};

	class LUST_API D3D12VertexBuffer: public VertexBuffer, public D3D12Buffer
	{
	public:
		D3D12VertexBuffer(const D3D12Context* context, const void* data, size_t size, uint32_t stride);
		~D3D12VertexBuffer();

		virtual void Stage() const override;

	private:
		D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;
	};

	class LUST_API D3D12IndexBuffer : public IndexBuffer, public D3D12Buffer
	{
	public:
		D3D12IndexBuffer(const D3D12Context* context, const void* data, size_t count);
		~D3D12IndexBuffer();

		virtual void Stage() const override;
		virtual uint32_t GetCount() const override;

	private:
		D3D12_INDEX_BUFFER_VIEW m_IndexBufferView;
	};
}

#endif
