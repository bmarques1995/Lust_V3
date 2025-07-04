#pragma once

#include "ShaderReflector.hpp"
#include <spirv_reflect.h>
#include "RawBuffer.hpp"

namespace Lust
{
	class VKShaderReflector : public ShaderReflector
	{
	public:
		VKShaderReflector(std::string_view jsonFilepath, uint32_t stages, uint32_t numInstances);
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

#ifdef LUST_DEBUG_MODE
		static size_t GetStructuredBufferStrideRecursively(const SpvReflectTypeDescription* type_desc);
#endif // 
		static size_t GetStructuredBufferStride(const SpvReflectTypeDescription* type_desc);

		static ShaderDataType CastToShaderDataType(SpvReflectFormat format);

		std::unordered_map<std::string, RawBuffer> m_ShaderBlobs;
		std::unordered_map<std::string, SpvReflectShaderModule> m_ShaderReflections;
	};
}