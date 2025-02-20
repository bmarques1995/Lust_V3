#pragma once

#include "LustShaderManagerDLLMacro.hpp"
#include <cstdint>
#include <initializer_list>
#include <unordered_map>
#include "UniformsLayout.hpp"

namespace Lust
{
	class LUST_SHADER_MNG_API StructuredBufferElement
	{
	public:
		StructuredBufferElement();
		StructuredBufferElement(uint32_t bindingSlot, uint32_t shaderRegister, uint32_t spaceSet, uint32_t bufferIndex, size_t stride, size_t numberOfBuffers, AccessLevel accessLevel, size_t bufferAlignment, uint32_t numberOfElements = 1);
		~StructuredBufferElement();

		const uint8_t* GetRawBuffer() const;

		BufferType GetBufferType() const;

		uint32_t GetBindingSlot() const;
		uint32_t GetShaderRegister() const;
		uint32_t GetSpaceSet() const;
		uint32_t GetBufferIndex() const;

		uint32_t GetNumberOfElements() const;

		AccessLevel GetAccessLevel() const;

		void SetBuffer(uint8_t** buffer);

		size_t GetStride() const;
		size_t GetNumberOfBuffers() const;
		size_t GetSize() const;
		size_t GetBufferAlignment() const;

		void CopyToBuffer(const void* buffer, size_t size, size_t offset) const;
	private:
		size_t m_Stride;
		size_t m_NumberOfBuffers;
		size_t m_BufferAlignment;
		uint8_t** m_Buffer;
		AccessLevel m_AccessLevel;
		uint32_t m_SpaceSet;
		uint32_t m_BindingSlot;
		uint32_t m_ShaderRegister;
		uint32_t m_BufferIndex;
		uint32_t m_NumberOfElements;
	};

	class LUST_SHADER_MNG_API StructuredBufferLayout
	{
	public:
		StructuredBufferLayout(std::initializer_list<StructuredBufferElement> elements, uint32_t allowedStages);

		const StructuredBufferElement& GetElement(uint32_t shaderRegister, uint32_t textureIndex) const;
		const std::unordered_map<uint64_t, StructuredBufferElement>& GetElements() const;

		StructuredBufferElement* GetElementPointer(uint32_t shaderRegister, uint32_t textureIndex);

		uint32_t GetStages() const;

	private:
		std::unordered_map<uint64_t, StructuredBufferElement> m_StructuredBuffers;
		uint32_t m_Stages;
		static StructuredBufferElement s_EmptyElement;
	};
}
