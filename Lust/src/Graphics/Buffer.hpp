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
}
