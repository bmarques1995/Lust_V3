#pragma once

#include "LustShaderManagerDLLMacro.hpp"
#include <cstdint>
#include <initializer_list>
#include <unordered_map>
#include <string>
#include "CommonResourceDefs.hpp"

namespace Lust
{
	/**
	* @brief Structured Buffer Element
	* @details The structured Buffer passes large amounts of data, structured, or
	* in vulkan naming, is the Storage Buffer
	*/
	class LUST_SHADER_MNG_API StructuredBufferElement
	{
	public:
		/**
		* @brief Default constructor
		*/
		StructuredBufferElement();
		/**
		* @brief Constructor
		* @param bindingSlot binding slot (Vulkan only), "binding", e.g. binding = 0
		* @param spaceSet descriptor set (Vulkan only), "set", e.g. set = 0
		* @param shaderRegister root signature element that describes the sampler (D3D12 only), "root Signature Index",
		* @param bufferIndex offset in the root signature (D3D12 only), "bufferIndex", e.g. bufferIndex = 0 means the first buffer in the root signature
		*/
		StructuredBufferElement(uint32_t bindingSlot, uint32_t shaderRegister, uint32_t spaceSet, uint32_t bufferIndex, size_t stride, AccessLevel accessLevel, size_t bufferAlignment, std::string name, uint32_t numberOfElements = 1);
		/**
		* @brief Default destructor
		*/
		~StructuredBufferElement();

		/**
		* @brief Returns the name of the structured buffer
		*/
		const std::string& GetName() const;
		/**
		* @brief Returns the type of the structured buffer
		*/
		BufferType GetBufferType() const;
		/**
		* @brief Returns the binding slot
		*/
		uint32_t GetBindingSlot() const;
		/**
		* @brief Returns the shader register
		*/
		uint32_t GetShaderRegister() const;
		/**
		* @brief Returns the descriptor set
		*/
		uint32_t GetSpaceSet() const;
		/**
		* @brief Returns the buffer index
		*/
		uint32_t GetBufferIndex() const;

		/**
		* @brief Returns the number of elements
		*/
		uint32_t GetNumberOfElements() const;
		/**
		* @brief Sets the number of elements
		*/
		void SetNumberOfElements(uint32_t numberOfElements) const;
		/**
		* @brief Returns the access level
		*/
		AccessLevel GetAccessLevel() const;

		/**
		* @brief Returns the stride, that corresponds to the size of one element
		*/
		size_t GetStride() const;
		/**
		* @brief Returns the size of the buffer, that corresponds to the size of all elements + alignment
		*/
		size_t GetSize() const;
		/**
		* @brief Returns the alignment of the buffer. The buffer needs to match the Vulkan and D3D12 alignment, so is adjusted for this
		*/
		size_t GetBufferAlignment() const;
		/**
		* @brief Returns the correction of the buffer, a value that is added to the buffer size to match the alignment
		*/
		size_t GetBufferCorrection() const;

	private:
		/**
		* @brief Recalculates the buffer alignment, if the amount of elements has changed
		*/
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

	/**
	* @brief Structured Buffer Layout
	* @details Represents a collection of structured buffer elements
	*/
	class LUST_SHADER_MNG_API StructuredBufferLayout
	{
	public:
		/**
		* @brief Constructor
		* @param elements elements of the layout
		* @param allowedStages allowed stages
		*/
		StructuredBufferLayout(std::initializer_list<StructuredBufferElement> elements, uint32_t allowedStages);

		/**
		* @brief Returns the element
		* @param elementName name(key) of the element
		* @return element
		*/
		const StructuredBufferElement& GetElement(std::string elementName) const;
		/**
		* @brief Returns the elements
		* @return elements
		*/
		const std::unordered_map<std::string, StructuredBufferElement>& GetElements() const;
		/**
		* @brief Returns a pointer to the element
		* @param elementName name(key) of the element
		* @return pointer to the element
		*/
		StructuredBufferElement* GetElementPointer(std::string elementName);
		/**
		* @brief Returns the allowed stages
		*/
		uint32_t GetStages() const;

		/**
		* @brief Clears the layout
		*/
		void Clear();
		/**
		* @brief Uploads the element to the layout
		*/
		void Upload(const StructuredBufferElement& element);

	private:
		std::unordered_map<std::string, StructuredBufferElement> m_StructuredBuffers;
		uint32_t m_Stages;
		static StructuredBufferElement s_EmptyElement;
	};
}
