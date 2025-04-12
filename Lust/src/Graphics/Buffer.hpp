#pragma once

#include "LustDLLMacro.hpp"
#include <cstdint>
#include <memory>
#include "GraphicsContext.hpp"

namespace Lust
{
	/**
	* @brief Vertex buffer interface
	*/
	class LUST_API VertexBuffer
	{
	public:
		/**
		* @brief Vertex buffer destructor
		*/
		virtual ~VertexBuffer() = default;

		/**
		* @brief Stage vertex buffer
		*/
		virtual void Stage() const = 0;

		/**
		* @brief Remap vertex buffer, upload data
		* @param data Vertex data
		* @param size Vertex data size
		*/
		virtual void Remap(const void* data, size_t size) = 0;

		/**
		* @brief Vertex buffer factory
		* @param context Graphics context interface
		* @param data Vertex data
		* @param size Vertex data size
		* @param stride Vertex stride
		*/
		static VertexBuffer* Instantiate(const GraphicsContext* context, const void* data, size_t size, uint32_t stride);
	protected:
		uint32_t m_Stride;
	};

	/**
	* @brief Index buffer interface
	*/
	class LUST_API IndexBuffer
	{
	public:
		/**
		* @brief Index buffer destructor
		*/
		virtual ~IndexBuffer() = default;

		/**
		* @brief Stage index buffer
		*/
		virtual void Stage() const = 0;
		/**
		* @brief Get index count
		*/
		virtual uint32_t GetCount() const = 0;

		/**
		* @brief Remap index buffer, upload data
		* @param data Index data
		* @param count Index data count
		*/
		virtual void Remap(const void* data, uint32_t count) = 0;
		
		/**
		* @brief Index buffer factory
		* @param context Graphics context interface
		* @param data Index data
		* @param count Index data count
		*/
		static IndexBuffer* Instantiate(const GraphicsContext* context, const void* data, uint32_t count);

	protected:
		uint32_t m_Count;
	};

	/**
	* @brief Uniform/constant buffer interface
	*/
	class LUST_API UniformBuffer
	{
	public:
		/**
		* @brief Uniform/constant buffer destructor
		*/
		virtual ~UniformBuffer() = default;

		/**
		* @brief Remap uniform/constant buffer, upload data
		*/
		virtual void Remap(const void* data, size_t size) = 0;
		/**
		* @brief Get uniform/constant buffer size
		*/
		virtual size_t GetSize() const = 0;

		/**
		* @brief Uniform/constant buffer factory
		* @param context Graphics context interface
		* @param data Uniform/constant buffer data
		* @param size Uniform/constant buffer data size
		*/
		static UniformBuffer* Instantiate(const GraphicsContext* context, const void* data, size_t size);
	protected:
		size_t m_BufferSize;
	};

	/**
	* @brief Structured buffer interface
	*/
	class LUST_API StructuredBuffer
	{
	public:
		/**
		* @brief Structured buffer destructor
		*/
		virtual ~StructuredBuffer() = default;

		/**
		* @brief Remap structured buffer, upload data
		* @param data Structured buffer data
		* @param size Structured buffer data size
		* @param offset Structured buffer data offset
		*/
		virtual void Remap(const void* data, size_t size, size_t offset) = 0;
		/**
		* @brief Get structured buffer size
		*/
		virtual size_t GetSize() const = 0;

		/**
		* @brief Structured buffer factory
		* @param context Graphics context interface
		* @param data Structured buffer data
		* @param size Structured buffer data size
		*/
		static StructuredBuffer* Instantiate(const GraphicsContext* context, const void* data, size_t size);
	protected:
		size_t m_BufferSize;
	};
}
