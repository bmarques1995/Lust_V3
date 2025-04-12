#pragma once

#ifdef LUST_USES_WINDOWS

#include "D3D12Context.hpp"
#include "Buffer.hpp"
#include <memory>

namespace Lust
{
	class LUST_API D3D12Buffer
	{
	public:
		ID3D12Resource2* GetResource() const;
	protected:
		D3D12Buffer(const D3D12Context* context);
		void CreateBuffer(const void* data, size_t size);
		void DestroyBuffer();
		void RemapBuffer(const void* data, size_t size, size_t offset);

		bool IsBufferConformed(size_t size);

		ComPointer<ID3D12Resource2> m_Buffer;
		const D3D12Context* m_Context;
		uint8_t* m_GPUData;
	};

	class LUST_API D3D12VertexBuffer: public VertexBuffer, public D3D12Buffer
	{
	public:
		D3D12VertexBuffer(const D3D12Context* context, const void* data, size_t size, uint32_t stride);
		~D3D12VertexBuffer();

		void Stage() const override;

		void Remap(const void* data, size_t size) override;

	private:
		D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;
	};

	class LUST_API D3D12IndexBuffer : public IndexBuffer, public D3D12Buffer
	{
	public:
		D3D12IndexBuffer(const D3D12Context* context, const void* data, uint32_t count);
		~D3D12IndexBuffer();

		virtual void Stage() const override;
		virtual uint32_t GetCount() const override;

		void Remap(const void* data, uint32_t count) override;
	private:
		D3D12_INDEX_BUFFER_VIEW m_IndexBufferView;
	};

	class LUST_API D3D12UniformBuffer : public UniformBuffer, public D3D12Buffer
	{
	public:
		D3D12UniformBuffer(const D3D12Context* context, const void* data, size_t size);
		~D3D12UniformBuffer();

		void Remap(const void* data, size_t size) override;
		size_t GetSize() const override;
	};

	class LUST_API D3D12StructuredBuffer : public StructuredBuffer, public D3D12Buffer
	{
	public:
		D3D12StructuredBuffer(const D3D12Context* context, const void* data, size_t size);
		~D3D12StructuredBuffer();

		void Remap(const void* data, size_t size, size_t offset) override;
		size_t GetSize() const override;
	};
}

#endif
