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
		StructuredBufferElement(uint32_t bindingSlot, uint32_t shaderRegister, uint32_t spaceSet, uint32_t bufferIndex, size_t stride, AccessLevel accessLevel, size_t bufferAlignment, std::string name, uint32_t numberOfElements = 1);
		~StructuredBufferElement();

		const std::string& GetName() const;
		BufferType GetBufferType() const;

		uint32_t GetBindingSlot() const;
		uint32_t GetShaderRegister() const;
		uint32_t GetSpaceSet() const;
		uint32_t GetBufferIndex() const;

		uint32_t GetNumberOfElements() const;
		void SetNumberOfElements(uint32_t numberOfElements) const;
		AccessLevel GetAccessLevel() const;


		size_t GetStride() const;
		size_t GetSize() const;
		size_t GetBufferAlignment() const;

	private:
		void RecalculateBufferAlignment() const;

		size_t m_Stride;
		mutable size_t m_BufferAlignment;
		mutable size_t m_BufferCorrection;
		AccessLevel m_AccessLevel;
		uint32_t m_SpaceSet;
		uint32_t m_BindingSlot;
		uint32_t m_ShaderRegister;
		uint32_t m_BufferIndex;
		mutable uint32_t m_NumberOfElements;
		std::string m_Name;
	};

	class LUST_SHADER_MNG_API StructuredBufferLayout
	{
	public:
		StructuredBufferLayout(std::initializer_list<StructuredBufferElement> elements, uint32_t allowedStages);

		const StructuredBufferElement& GetElement(std::string elementName) const;
		const std::unordered_map<std::string, StructuredBufferElement>& GetElements() const;

		StructuredBufferElement* GetElementPointer(std::string elementName);

		uint32_t GetStages() const;

		void Clear();
		void Upload(const StructuredBufferElement& element);

	private:
		std::unordered_map<std::string, StructuredBufferElement> m_StructuredBuffers;
		uint32_t m_Stages;
		static StructuredBufferElement s_EmptyElement;
	};
}
