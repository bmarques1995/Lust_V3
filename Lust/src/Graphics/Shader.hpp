#pragma once

#include "LustDLLMacro.hpp"
#include "InputBufferLayout.hpp"
#include "UniformsLayout.hpp"
#include "SamplerLayout.hpp"
#include "TextureLayout.hpp"
#include "StructuredBufferLayout.hpp"
#include "Topology.hpp"
#include "Texture.hpp"
#include "Buffer.hpp"
#include "GraphicsContext.hpp"
#include <json/json.h>


namespace Lust
{
	class LUST_API SizeMismatchException : public GraphicsException
	{
	public:
		SizeMismatchException(size_t layoutSize, size_t providedSize);
	};

	struct LUST_API InputInfo
	{
		InputBufferLayout m_InputLayout;
		SmallBufferLayout m_SmallBufferLayout;
		UniformLayout m_UniformLayout;
		TextureLayout m_TextureLayout;
		SamplerLayout m_SamplerLayout;
		StructuredBufferLayout m_StructuredBufferLayout;
		Topology m_Topology = Topology::LUST_TOPOLOGY_TRIANGLELIST;

		InputInfo(InputBufferLayout inputLayout, SmallBufferLayout smallBufferLayout, UniformLayout uniformLayout, TextureLayout textureLayout, SamplerLayout samplerLayout, StructuredBufferLayout structuredBufferLayout);
	};

	class LUST_API Shader
	{
	public:
		Shader(const InputInfo& inputInfo);

		const InputBufferLayout& GetInputLayout() const;
		const SmallBufferLayout& GetSmallBufferLayout() const;
		const UniformLayout& GetUniformLayout() const;
		const TextureLayout& GetTextureLayout() const;
		const SamplerLayout& GetSamplerLayout() const;

		virtual ~Shader() = default;
		virtual void Stage() = 0;
		virtual uint32_t GetStride() const = 0;
		virtual uint32_t GetOffset() const = 0;

		const std::unordered_map<uint64_t, TextureElement>& GetTextureElements() const;
		virtual void UploadTexture2D(const std::shared_ptr<Texture2D>* texture, const TextureElement& textureElement) = 0;

		const std::unordered_map<uint32_t, UniformElement>& GetUniformElements() const;
		virtual void UploadConstantBuffer(const std::shared_ptr<UniformBuffer>* buffer, const UniformElement& uploadCBV) = 0;

		virtual void BindSmallBuffer(const void* data, size_t size, uint32_t bindingSlot, size_t offset) = 0;
		virtual void BindDescriptors() = 0;
		virtual void UpdateSSBO(const StructuredBufferElement& uploadBuffer) = 0;

		static Shader* Instantiate(const GraphicsContext* context, std::string json_basepath, const InputInfo& inputInfo);

	protected:

		Json::Value m_PipelineInfo;
		std::string m_ShaderDir;

		InputBufferLayout m_Layout;
		SmallBufferLayout m_SmallBufferLayout;
		UniformLayout m_UniformLayout;
		TextureLayout m_TextureLayout;
		SamplerLayout m_SamplerLayout;
		StructuredBufferLayout m_StructuredBufferLayout;

		static void InitJsonAndPaths(std::string json_controller_path, Json::Value* pipelineInfo, std::string* shaderDir);
	};
}