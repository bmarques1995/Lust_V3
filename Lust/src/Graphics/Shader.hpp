#pragma once

#include "LustDLLMacro.hpp"
#include "InputBufferLayout.hpp"
#include "UniformsLayout.hpp"
#include "SamplerLayout.hpp"
#include "TextureLayout.hpp"
#include "Texture.hpp"
#include "GraphicsContext.hpp"
#include <json/json.h>

namespace Lust
{
	class LUST_API SizeMismatchException : public GraphicsException
	{
	public:
		SizeMismatchException(size_t layoutSize, size_t providedSize);
	};

	class LUST_API Shader
	{
	public:
		Shader(InputBufferLayout layout, SmallBufferLayout smallBufferLayout, UniformLayout uniformLayout, TextureLayout textureLayout, SamplerLayout samplerLayout);
		virtual ~Shader() = default;
		virtual void Stage() = 0;
		virtual uint32_t GetStride() const = 0;
		virtual uint32_t GetOffset() const = 0;

		virtual void UploadTexture2D(const std::shared_ptr<Texture2D>* texture) = 0;

		virtual void BindSmallBuffer(const void* data, size_t size, uint32_t bindingSlot) = 0;
		virtual void BindDescriptors() = 0;
		virtual void UpdateCBuffer(const void* data, size_t size, uint32_t shaderRegister, uint32_t tableIndex) = 0;

		static Shader* Instantiate(const std::shared_ptr<GraphicsContext>* context, std::string json_basepath, InputBufferLayout layout, SmallBufferLayout smallBufferLayout, UniformLayout uniformLayout, TextureLayout textureLayout, SamplerLayout samplerLayout);
	
	protected:

		Json::Value m_PipelineInfo;
		std::string m_ShaderDir;

		InputBufferLayout m_Layout;
		SmallBufferLayout m_SmallBufferLayout;
		UniformLayout m_UniformLayout;
		TextureLayout m_TextureLayout;
		SamplerLayout m_SamplerLayout;

		static void InitJsonAndPaths(std::string json_controller_path, Json::Value* pipelineInfo, std::string* shaderDir);
	};
}