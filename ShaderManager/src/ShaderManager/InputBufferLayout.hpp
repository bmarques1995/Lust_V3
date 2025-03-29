#pragma once

#include "LustShaderManagerDLLMacro.hpp"
#include <cstdint>
#include <cassert>
#include <string>
#include "Console.hpp"

namespace Lust
{
	/**
	* @brief Enum for shader data types
	* @param None represents no data type
	* @param Float represents a float
	* @param Float2 represents a float2
	* @param Float3 represents a float3
	* @param Float4 represents a float4
	* @param Uint represents an uint
	* @param Uint2 represents an uint2
	* @param Uint3 represents an uint3
	* @param Uint4 represents an uint4
	*/
	enum class LUST_SHADER_MNG_API ShaderDataType
	{
		None = 0, Float, Float2, Float3, Float4, Uint, Uint2, Uint3, Uint4
	};

	/**
	* @brief Returns the size of a shader data type
	* @param type shader data type
	* @return size of the shader data type
	*/
	LUST_SHADER_MNG_API uint32_t ShaderDataTypeSize(ShaderDataType type);
	
	/**
	* @brief Class for input assembler elements
	*/
	class LUST_SHADER_MNG_API InputBufferElement
	{
		friend class InputBufferLayout;
		friend class Shader;
	public:
		/**
		* @brief Default constructor
		*/
		InputBufferElement();

		/**
		* @brief Constructor
		* @param type shader data type
		* @param name name of the element
		* @param normalized if the element is normalized
		*/
		InputBufferElement(ShaderDataType type, const std::string& name, bool normalized = false);

		/**
		* @brief Returns the component count of the shader data type
		* @details Returns the number of components of the type, e.g. Float2 returns 2
		* @return component count
		*/
		uint32_t GetComponentCount() const;

		/**
		* @brief Returns the name of the element
		* @return name of the element
		*/
		const std::string& GetName() const;
		/**
		* @brief Returns the shader data type
		* @return shader data type
		*/
		const ShaderDataType GetType() const;
		/**
		* @brief Returns the size of the element
		* @return size of the element
		*/
		const uint32_t GetSize() const;
		/**
		* @brief Returns the offset of the element
		* @return offset of the element
		*/
		const uint32_t GetOffset() const;
		/**
		* @brief Returns if the element is normalized
		* @return if the element is normalized
		*/
		const bool IsNormalized() const;
	private:
		std::string m_Name;
		ShaderDataType m_Type;
		uint32_t m_Size;
		uint32_t m_Offset;
		bool m_Normalized;
	};

	/**
	* @brief Class for encapsulate all input assembler elements
	*/
	class LUST_SHADER_MNG_API InputBufferLayout
	{
	public:
		/**
		* @brief Default constructor
		*/
		InputBufferLayout() {}

		/**
		* @brief Constructor
		* @param elements elements of the layout
		*/
		InputBufferLayout(const std::initializer_list<InputBufferElement>& elements);

		/**
		* @brief Returns the stride of the layout
		* @return stride of the layout
		*/
		inline uint32_t GetStride() const { return m_Stride; }
		/**
		* @brief Returns the elements of the layout
		* @return elements of the layout
		*/
		inline const std::vector<InputBufferElement>& GetElements() const { return m_Elements; }
		
		/**
		* @brief Clears the layout
		*/
		void Clear();
		/**
		* @brief Adds an element to the layout
		* @param element element to add
		*/
		void PushBack(const InputBufferElement& element);

		/**
		* @brief Returns an iterator to the beginning of the elements
		* @return iterator to the beginning of the elements
		*/
		std::vector<InputBufferElement>::iterator begin() { return m_Elements.begin(); }
		/**
		* @brief Returns an iterator to the end of the elements
		* @return iterator to the end of the elements
		*/
		std::vector<InputBufferElement>::iterator end() { return m_Elements.end(); }
		/**
		* @brief Returns an iterator to the beginning of the elements
		* @return iterator to the beginning of the elements
		*/
		std::vector<InputBufferElement>::const_iterator const_begin() const { return m_Elements.begin(); }
		/**
		* @brief Returns an iterator to the end of the elements
		* @return iterator to the end of the elements
		*/
		std::vector<InputBufferElement>::const_iterator const_end() const { return m_Elements.end(); }
	private:
		/**
		* @brief Calculates the offsets and stride of the elements
		* @details Offset is the start position of the element, stride is the size of all elements combined
		*/
		void CalculateOffsetsAndStride();
		
	private:
		std::vector<InputBufferElement> m_Elements;
		uint32_t m_Stride = 0;
	};
}