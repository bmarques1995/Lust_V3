#pragma once

#include <cstdint>
#include "CommonException.hpp"
#include <unordered_map>
#include "LustShaderManagerDLLMacro.hpp"

namespace Lust
{
	enum class LUST_SHADER_MNG_API BufferType
	{
		INVALID_BUFFER_TYPE,
		UNIFORM_CONSTANT_BUFFER,
		STRUCTURED_BUFFER,
		TEXTURE_BUFFER,
		SAMPLER_BUFFER
	};

	enum class AccessLevel
	{
		ROOT_BUFFER,
		DESCRIPTOR_BUFFER
	};

	class LUST_SHADER_MNG_API AttachmentMismatchException : public GraphicsException
	{
	public:
		AttachmentMismatchException(size_t bufferSize, size_t expectedBufferAttachment);
	};

	class LUST_SHADER_MNG_API SignatureMismatchException : public GraphicsException
	{
	public:
		SignatureMismatchException(uint32_t numberOfBuffers);
	};

	enum LUST_SHADER_MNG_API AllowedStages
	{
		EMPTY_STAGE = 1 >> 1,
		VERTEX_STAGE = 1 << 0,
		PIXEL_STAGE = 1 << 1,
		GEOMETRY_STAGE = 1 << 2,
		HULL_STAGE = 1 << 3,
		DOMAIN_STAGE = 1 << 4,
		MESH_STAGE = 1 << 5,
		AMPLIFICATION_STAGE = 1 << 6
	};

	class LUST_SHADER_MNG_API SmallBufferElement
	{
	public:
		SmallBufferElement();
		SmallBufferElement(size_t offset, size_t size, uint32_t bindingSlot, uint32_t smallAttachment, std::string name);

		size_t GetOffset() const;
		size_t GetSize() const;
		uint32_t GetBindingSlot() const;
		const std::string& GetName() const;

		bool IsSizeValid(uint32_t smallAttachment);

	private:

		std::string m_Name;
		size_t m_Offset;
		size_t m_Size;
		uint32_t m_BindingSlot;
	};

	class LUST_SHADER_MNG_API SmallBufferLayout
	{
	public:
		SmallBufferLayout(std::initializer_list<SmallBufferElement> m_Elements, uint32_t stages);

		const SmallBufferElement& GetElement(std::string_view name) const;
		const std::unordered_map<std::string, SmallBufferElement>& GetElements() const;
		uint32_t GetStages() const;

		void Clear();
		void Upload(const SmallBufferElement& element);
	private:
		std::unordered_map<std::string, SmallBufferElement> m_Buffers;
		uint32_t m_Stages;
		static SmallBufferElement s_EmptyElement;
	};

	class LUST_SHADER_MNG_API UniformElement
	{
	public:
		UniformElement();
		UniformElement(BufferType bufferType, size_t size, uint32_t bindingSlot, uint32_t spaceSet, uint32_t shaderRegister, AccessLevel accessLevel, uint32_t numberOfBuffers, uint32_t bufferAttachment, uint32_t tableIndex, std::string name);

		const std::string& GetName() const;

		BufferType GetBufferType() const;
		size_t GetSize() const;
		uint32_t GetBindingSlot() const;
		uint32_t GetShaderRegister() const;
		uint32_t GetSpaceSet() const;
		AccessLevel GetAccessLevel() const;
		uint32_t GetNumberOfBuffers() const;
		uint32_t GetTableIndex() const;

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

	class LUST_SHADER_MNG_API UniformLayout
	{
	public:
		UniformLayout(std::initializer_list<UniformElement> m_Elements, uint32_t allowedStages);

		const UniformElement& GetElement(std::string name) const;
		const std::unordered_map<std::string, UniformElement>& GetElements() const;
		uint32_t GetStages() const;

		void Clear();
		void Upload(const UniformElement& element);
	private:
		uint32_t m_Stages;
		std::unordered_map<std::string, UniformElement> m_Buffers;
		static UniformElement s_EmptyElement;
	};
}