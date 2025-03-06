#pragma once

#include "ShaderReflector.hpp"
#include <spirv_reflect.h>

namespace Lust
{
	class RawBuffer
	{
	public:
		RawBuffer();
		RawBuffer(uint8_t* data, size_t size);
		~RawBuffer();

		void RecreateBuffer(uint8_t* data, size_t size);

		const uint8_t* GetData() const;
		const size_t GetSize() const;

	private:
		uint8_t* m_Data;
		size_t m_Size;
	};
	class VKShaderReflector : public ShaderReflector
	{
	public:
		VKShaderReflector(std::string_view jsonFilepath);
		~VKShaderReflector();

	private:
		void GenerateInputBufferLayout();
		void UploadBlob(std::string_view shader_stage, RawBuffer* blob);
		void ReflectStage(std::string_view shader_stage, const RawBuffer& blob);

		static ShaderDataType CastToShaderDataType(SpvReflectFormat format);

		std::unordered_map<std::string, RawBuffer> m_ShaderBlobs;
		std::unordered_map<std::string, SpvReflectShaderModule> m_ShaderReflections;
	};
}