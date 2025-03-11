#pragma once

#include "LustDLLMacro.hpp"
#include "InputBufferLayout.hpp"
#include "UniformsLayout.hpp"
#include "SamplerLayout.hpp"
#include "TextureLayout.hpp"
#include "StructuredBufferLayout.hpp"
#include "Topology.hpp"
#include "Texture.hpp"
#include "Sampler.hpp"
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
		TextureArrayLayout m_TextureArrayLayout;
		SamplerArrayLayout m_SamplerArrayLayout;
		StructuredBufferLayout m_StructuredBufferLayout;
		Topology m_Topology = Topology::LUST_TOPOLOGY_TRIANGLELIST;
		
		InputInfo(uint32_t stages);
		InputInfo(InputBufferLayout inputLayout, SmallBufferLayout smallBufferLayout, UniformLayout uniformLayout, TextureLayout textureLayout, SamplerLayout samplerLayout, 
			TextureArrayLayout textureArrayLayout, SamplerArrayLayout samplerArrayLayout, StructuredBufferLayout structuredBufferLayout);
	};

	class LUST_API Shader
	{
	public:
		Shader(const InputInfo& inputInfo, const std::string& filepath);

		const InputBufferLayout& GetInputLayout() const;
		const SmallBufferLayout& GetSmallBufferLayout() const;
		const UniformLayout& GetUniformLayout() const;
		const StructuredBufferLayout& GetStructuredBufferLayout() const;
		const TextureLayout& GetTextureLayout() const;
		const SamplerLayout& GetSamplerLayout() const;
		const TextureArrayLayout& GetTextureArrayLayout() const;
		const SamplerArrayLayout& GetSamplerArrayLayout() const;

		virtual ~Shader() = default;
		virtual void Stage() = 0;
		virtual uint32_t GetStride() const = 0;
		virtual uint32_t GetOffset() const = 0;

		const std::string& GetName() const;
		void SetName(const std::string& name);

		virtual void UploadTexture2D(const std::shared_ptr<Texture2D>* texture, const TextureElement& textureElement) = 0;
		virtual void UploadTexture2D(const std::shared_ptr<Texture2D>* texture, const TextureArray& textureArray, uint32_t offset) = 0;
		virtual void UploadConstantBuffer(const std::shared_ptr<UniformBuffer>* buffer, const UniformElement& uploadCBV) = 0;

		virtual void UploadStructuredBuffer(const std::shared_ptr<StructuredBuffer>* buffer, const StructuredBufferElement& uploadSRV) = 0;

		virtual void UploadSampler(const std::shared_ptr<Sampler>* sampler, const SamplerElement& samplerElement) = 0;
		virtual void UploadSampler(const std::shared_ptr<Sampler>* sampler, const SamplerArray& samplerArray, uint32_t offset) = 0;

		virtual void BindSmallBuffer(const void* data, size_t size, const SmallBufferElement& smallBuffer, size_t offset) = 0;
		virtual void BindDescriptors() = 0;

		virtual void UploadTexturePackedDescSet(const TextureArray& textureArray) = 0;
		virtual void UploadSamplerPackedDescSet(const SamplerArray& samplerArray) = 0;

		static Shader* Instantiate(const GraphicsContext* context, std::string json_basepath, const InputInfo& inputInfo);

	protected:
		Json::Value m_PipelineInfo;
		std::string m_ShaderDir;
		std::string m_Name;

		InputBufferLayout m_Layout;
		SmallBufferLayout m_SmallBufferLayout;
		UniformLayout m_UniformLayout;
		TextureLayout m_TextureLayout;
		SamplerLayout m_SamplerLayout;
		TextureArrayLayout m_TextureArrayLayout;
		SamplerArrayLayout m_SamplerArrayLayout;
		StructuredBufferLayout m_StructuredBufferLayout;

		static void InitJsonAndPaths(std::string json_controller_path, Json::Value* pipelineInfo, std::string* shaderDir);
	};

	class LUST_API ShaderLibrary
	{
	public:
		ShaderLibrary(const GraphicsContext* context);
		~ShaderLibrary();

		void Add(const std::string& name, const std::shared_ptr<Shader>& shader);
		void Add(const std::shared_ptr<Shader>& shader);
		std::shared_ptr<Shader> Load(const std::string& json_basepath, const InputInfo& inputInfo);
		std::shared_ptr<Shader> Load(const std::string& name, const std::string& json_basepath, const InputInfo& inputInfo);

		std::shared_ptr<Shader> Get(const std::string& name);

		bool Exists(const std::string& name) const;
	private:
		std::unordered_map<std::string, std::shared_ptr<Shader>> m_Shaders;
		const GraphicsContext* m_Context;
	};
}