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
		void CreateBuffer(const void* data, size_t size, bool dynamicBuffer = false);
		void DestroyBuffer();
		void RemapCall(const void* data, size_t size, size_t offset);

		bool IsBufferConformed(size_t size);

		ComPointer<ID3D12Resource2> m_Buffer;
		ComPointer<D3D12MA::Allocation> m_BufferAllocation;
		const D3D12Context* m_Context;
		uint8_t* m_GPUData;
		bool m_IsDynamic;
	private:
		void RemapBuffer(const void* data, size_t size, size_t offset);
		void RemapBufferStaticly(const void* data, size_t size, size_t offset);
	};

	class LUST_API D3D12VertexBuffer: public VertexBuffer, public D3D12Buffer
	{
	public:
		D3D12VertexBuffer(const D3D12Context* context, const void* data, size_t size, uint32_t stride, bool dynamicBuffer);
		~D3D12VertexBuffer();

		void Stage() const override;

		void Remap(const void* data, size_t size) override;

	private:
		D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;
	};

	class LUST_API D3D12IndexBuffer : public IndexBuffer, public D3D12Buffer
	{
	public:
		D3D12IndexBuffer(const D3D12Context* context, const void* data, uint32_t count, bool dynamicBuffer);
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
