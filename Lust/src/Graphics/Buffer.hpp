#pragma once

#include "LustDLLMacro.hpp"
#include <cstdint>
#include <memory>
#include "GraphicsContext.hpp"

namespace Lust
{
	class LUST_API VertexBuffer
	{
	public:
		virtual ~VertexBuffer() = default;

		virtual void Stage() const = 0;
		static VertexBuffer* Instantiate(const GraphicsContext* context, const void* data, size_t size, uint32_t stride);
	protected:
		uint32_t m_Stride;
	};

	class LUST_API IndexBuffer
	{
	public:
		virtual ~IndexBuffer() = default;

		virtual void Stage() const = 0;
		virtual uint32_t GetCount() const = 0;

		static IndexBuffer* Instantiate(const GraphicsContext* context, const void* data, size_t count);

	protected:
		uint32_t m_Count;
	};

	class LUST_API UniformBuffer
	{
	public:
		virtual ~UniformBuffer() = default;

		virtual void Remap(const void* data, size_t size) = 0;
		virtual size_t GetSize() const = 0;

		static UniformBuffer* Instantiate(const GraphicsContext* context, const void* data, size_t size);
	protected:
		size_t m_BufferSize;
	};

	class LUST_API StructuredBuffer
	{
	public:
		virtual ~StructuredBuffer() = default;

		virtual void Remap(const void* data, size_t size, size_t offset) = 0;
		virtual size_t GetSize() const = 0;

		static StructuredBuffer* Instantiate(const GraphicsContext* context, const void* data, size_t size);
	protected:
		size_t m_BufferSize;
	};
}
