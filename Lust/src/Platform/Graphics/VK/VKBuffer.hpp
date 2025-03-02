#pragma once

#include "VKContext.hpp"
#include "Buffer.hpp"
#include <memory>

namespace Lust
{
	class LUST_API VKBuffer
	{
	public:
		VkBuffer GetBuffer() const;
		VkDeviceMemory GetMemory() const;
	protected:
		VKBuffer(const VKContext* context);
		void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
		void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		void RemapBuffer(const void* data, size_t size, size_t offset);
		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

		bool IsBufferConformed(size_t size);
		void DestroyBuffer();

		const VKContext* m_Context;
		VkBuffer m_Buffer;
		VkDeviceMemory m_BufferMemory;
	};

	class LUST_API VKVertexBuffer : public VertexBuffer, public VKBuffer
	{
	public:
		VKVertexBuffer(const VKContext* context, const void* data, size_t size, uint32_t stride);
		~VKVertexBuffer();

		virtual void Stage() const override;

	private:

	};

	class LUST_API VKIndexBuffer : public IndexBuffer, public VKBuffer
	{
	public:
		VKIndexBuffer(const VKContext* context, const void* data, size_t count);
		~VKIndexBuffer();

		virtual void Stage() const override;
		virtual uint32_t GetCount() const override;

	private:

	};

	class LUST_API VKUniformBuffer : public UniformBuffer, public VKBuffer
	{
	public:
		VKUniformBuffer(const VKContext* context, const void* data, size_t size);
		~VKUniformBuffer();

		void Remap(const void* data, size_t size) override;
		size_t GetSize() const override;
	};

	class LUST_API VKStructuredBuffer : public StructuredBuffer, public VKBuffer
	{
	public:
		VKStructuredBuffer(const VKContext* context, const void* data, size_t size);
		~VKStructuredBuffer();

		void Remap(const void* data, size_t size, size_t offset) override;
		size_t GetSize() const override;
	};
}
