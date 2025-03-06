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
		VKShaderReflector(std::string_view jsonFilepath, uint32_t stages);
		~VKShaderReflector();

	private:
		void GenerateInputBufferLayout();
		void GenerateSmallBufferLayout(const SpvReflectShaderModule* module);
		void GenerateBuffersLayout(const SpvReflectShaderModule* module);
		void UploadBlob(std::string_view shader_stage, RawBuffer* blob);
		void ReflectStage(std::string_view shader_stage, const RawBuffer& blob);

		static void CreateUniformElement(SpvReflectDescriptorBinding** reflector_binder, VKShaderReflector* instance);
		static void CreateTextureElement(SpvReflectDescriptorBinding** reflector_binder, VKShaderReflector* instance);
		static void CreateSamplerElement(SpvReflectDescriptorBinding** reflector_binder, VKShaderReflector* instance);
		static void CreateStructuredBufferElement(SpvReflectDescriptorBinding** reflector_binder, VKShaderReflector* instance);

		static ShaderDataType CastToShaderDataType(SpvReflectFormat format);

		std::unordered_map<std::string, RawBuffer> m_ShaderBlobs;
		std::unordered_map<std::string, SpvReflectShaderModule> m_ShaderReflections;
	};
}