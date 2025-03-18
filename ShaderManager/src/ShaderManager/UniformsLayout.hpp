#pragma once

#include <cstdint>
#include "CommonException.hpp"
#include <unordered_map>
#include "LustShaderManagerDLLMacro.hpp"
#include "CommonResourceDefs.hpp"

namespace Lust
{
	/**
	* @brief Exception thrown when the number of attachments does not match the number of buffers
	*/
	class LUST_SHADER_MNG_API AttachmentMismatchException : public GraphicsException
	{
	public:
		/**
		* @brief Constructor
		* @param bufferSize number of buffers
		* @param expectedBufferAttachment number of attachments
		*/
		AttachmentMismatchException(size_t bufferSize, size_t expectedBufferAttachment);
	};

	/**
	* @brief Exception thrown when the number of buffers does not match the number of root signature elements.
	* For root elements, the number of buffers is 1
	*/
	class LUST_SHADER_MNG_API SignatureMismatchException : public GraphicsException
	{
	public:
		/**
		* @brief Constructor
		* @param numberOfBuffers number of buffers
		*/
		SignatureMismatchException(uint32_t numberOfBuffers);
	};

	/**
	* @brief Small buffer element, to pass small amounts of data, using push constants(vulkan) or root 32 bit constants(D3D12)
	*/
	class LUST_SHADER_MNG_API SmallBufferElement
	{
	public:
		/**
		* @brief Default constructor
		*/
		SmallBufferElement();
		/**
		* @brief Constructor
		* @param offset offset in the buffer
		* @param size size of the element
		* @param bindingSlot binding slot (Vulkan only), "binding", e.g. binding = 0
		* @param smallAttachment offset in the root signature (D3D12 only), "small Attachment", e.g. smallAttachment = 0 means the first small buffer in the root signature
		* @param name name
		*/
		SmallBufferElement(size_t offset, size_t size, uint32_t bindingSlot, uint32_t smallAttachment, std::string name);

		/**
		* @brief Returns the offset
		*/
		size_t GetOffset() const;
		/**
		* @brief Returns the size
		*/
		size_t GetSize() const;
		/**
		* @brief Returns the binding slot
		*/
		uint32_t GetBindingSlot() const;
		/**
		* @brief Returns the name
		*/
		const std::string& GetName() const;

		/**
		* @brief Checks if the size is valid
		*/
		bool IsSizeValid(uint32_t smallAttachment);

	private:

		std::string m_Name;
		size_t m_Offset;
		size_t m_Size;
		uint32_t m_BindingSlot;
	};

	/**
	* @brief Small buffer layout, handler for small buffer elements
	*/
	class LUST_SHADER_MNG_API SmallBufferLayout
	{
	public:
		/**
		* @brief Constructor
		* @param elements elements of the layout
		* @param stages allowed stages
		*/
		SmallBufferLayout(std::initializer_list<SmallBufferElement> m_Elements, uint32_t stages);

		/**
		* @brief Returns the element of the layout
		* @param name name(key) of the element
		*/
		const SmallBufferElement& GetElement(std::string_view name) const;
		/**
		* @brief Returns the elements of the layout
		*/
		const std::unordered_map<std::string, SmallBufferElement>& GetElements() const;
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
		void Upload(const SmallBufferElement& element);
	private:
		std::unordered_map<std::string, SmallBufferElement> m_Buffers;
		uint32_t m_Stages;
		static SmallBufferElement s_EmptyElement;
	};

	/**
	* @brief Uniform element, a container for a uniform/constant buffer
	*/
	class LUST_SHADER_MNG_API UniformElement
	{
	public:
		/**
		* @brief Default constructor
		*/
		UniformElement();
		/**
		* @brief Constructor
		* @param bufferType type of the buffer
		* @param size size of the buffer
		* @param bindingSlot binding slot (Vulkan only), "binding", e.g. binding = 0
		* @param spaceSet descriptor set (Vulkan only), "set", e.g. set = 0
		* @param shaderRegister root signature element that describes the buffer (D3D12 only), "root Signature Index",
		* @param accessLevel access level (D3D12 only), "access Level", @see AccessLevel
		* @param numberOfBuffers number of buffers (D3D12 only), "number of Buffers"
		* @param bufferAttachment verifies if the buffer size complies the API requirements
		* @param name name
		*/
		UniformElement(BufferType bufferType, size_t size, uint32_t bindingSlot, uint32_t spaceSet, uint32_t shaderRegister, AccessLevel accessLevel, uint32_t numberOfBuffers, uint32_t bufferAttachment, uint32_t tableIndex, std::string name);

		/**
		* @brief Returns the name
		*/
		const std::string& GetName() const;

		/**
		* @brief Returns the buffer type
		*/
		BufferType GetBufferType() const;
		/**
		* @brief Returns the size
		*/
		size_t GetSize() const;
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
		* @brief Returns the access level
		*/
		AccessLevel GetAccessLevel() const;
		/**
		* @brief Returns the number of buffers
		*/
		uint32_t GetNumberOfBuffers() const;
		/**
		* @brief Returns the buffer index
		*/
		uint32_t GetTableIndex() const;

		/**
		* @brief Checks if the size is valid
		*/
		bool IsSizeValid(uint32_t bufferAttachment);

	private:

		std::string m_Name;

		BufferType m_BufferType;
		size_t m_Size;

		uint32_t m_BindingSlot;
		uint32_t m_ShaderRegister;
		uint32_t m_SpaceSet;
		AccessLevel m_AccessLevel;
		uint32_t m_NumberOfBuffers;
		uint32_t m_TableIndex;
	};

	/***/
	class LUST_SHADER_MNG_API UniformLayout
	{
	public:
		/**
		* @brief Constructor
		* @param elements elements of the layout
		* @param stages allowed stages
		*/
		UniformLayout(std::initializer_list<UniformElement> m_Elements, uint32_t allowedStages);

		/**
		* @brief Returns the element of the layout
		* @param name name(key) of the element
		*/
		const UniformElement& GetElement(std::string name) const;
		/**
		* @brief Returns the elements of the layout
		*/
		const std::unordered_map<std::string, UniformElement>& GetElements() const;
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
		void Upload(const UniformElement& element);
	private:
		uint32_t m_Stages;
		std::unordered_map<std::string, UniformElement> m_Buffers;
		static UniformElement s_EmptyElement;
	};
}