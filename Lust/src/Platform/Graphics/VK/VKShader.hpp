#pragma once

#include "Shader.hpp"
#include "VKContext.hpp"
#include "VKTexture.hpp"
#include "VKSampler.hpp"
#include "VKBuffer.hpp"
#include "DXCSafeInclude.hpp"
#include <functional>

namespace Lust
{
	//Resource Memomy View
	struct RM
	{
		VkBuffer Resource;
		VkDeviceMemory Memory;
	};

	class LUST_API VKShader : public Shader
	{
	public:
		VKShader(const VKContext* context, std::string json_controller_path, const std::shared_ptr<ShaderReflector>& inputInfo, const Topology& topology);
		~VKShader();

		void Stage() override;
		uint32_t GetStride() const override;
		uint32_t GetOffset() const override;

		void UploadTexture2D(const std::shared_ptr<Texture2D>* texture, const TextureElement& textureElement) override;
		void UploadTexture2D(const std::shared_ptr<Texture2D>* texture, const TextureArray& textureArray, uint32_t offset) override;

		void UploadConstantBuffer(const std::shared_ptr<UniformBuffer>* buffer, const UniformElement& uploadCBV) override;
		void UploadStructuredBuffer(const std::shared_ptr<StructuredBuffer>* buffer, const StructuredBufferElement& uploadSRV) override;

		void UploadSampler(const std::shared_ptr<Sampler>* sampler, const SamplerElement& textureElement) override;
		void UploadSampler(const std::shared_ptr<Sampler>* sampler, const SamplerArray& samplerArray, uint32_t offset) override;

		void BindSmallBuffer(const void* data, size_t size, const SmallBufferElement& smallBuffer, size_t offset) override;
		void BindDescriptors() override;

		void UploadTexturePackedDescSet(const TextureArray& textureArray) override;
		void UploadSamplerPackedDescSet(const SamplerArray& samplerArray) override;
	private:

		void PreallocatesDescSets();

		void CreateDescriptorSetLayout();
		void CreateDescriptorPool();

		void CreateBuffer(size_t bufferSize, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer* buffer, VkDeviceMemory* bufferMemory);

		void CreateTextureDescriptorSet(const std::shared_ptr<VKTexture2D>* texture, const TextureElement& textureElement);
		void CreateTextureDescriptorSet(const std::shared_ptr<VKTexture2D>* texture, const TextureArray& textureArray, uint32_t offset);
		
		void CreateSamplerDescriptorSet(const std::shared_ptr<VKSampler>* sampler, const SamplerElement& samplerElement);
		void CreateSamplerDescriptorSet(const std::shared_ptr<VKSampler>* sampler, const SamplerArray& samplerArray, uint32_t offset);

		void CreateUniformDescriptorSet(const std::shared_ptr<VKUniformBuffer>* buffer, const UniformElement& uniformElement);
		void CreateStructuredBufferDescriptorSet(const std::shared_ptr<VKStructuredBuffer>* buffer, const StructuredBufferElement& uniformElement);

		bool IsUniformValid(size_t size);

		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

		void PushShader(std::string_view stage, VkPipelineShaderStageCreateInfo* graphicsDesc);
		void SetRasterizer(VkPipelineRasterizationStateCreateInfo* rasterizer);
		void SetInputAssemblyViewportAndMultisampling(VkPipelineInputAssemblyStateCreateInfo* inputAssembly, VkPipelineViewportStateCreateInfo* viewportState, VkPipelineMultisampleStateCreateInfo* multisampling, Topology topology);
		void SetBlend(VkPipelineColorBlendAttachmentState* colorBlendAttachment, VkPipelineColorBlendStateCreateInfo* colorBlending);
		void SetDepthStencil(VkPipelineDepthStencilStateCreateInfo* depthStencil);

		static VkFormat GetNativeFormat(ShaderDataType type);
		static VkDescriptorType GetNativeDescriptorType(BufferType type);
		static VkBufferUsageFlagBits GetNativeBufferUsage(BufferType type);
		static VkPrimitiveTopology GetNativeTopology(Topology topology);

		static const std::list<std::string> s_GraphicsPipelineStages;

		static const std::unordered_map<std::string, VkShaderStageFlagBits> s_StageCaster;
		static const std::unordered_map<uint32_t, VkShaderStageFlagBits> s_EnumStageCaster;

		std::unordered_map<std::string, VkShaderModule> m_Modules;
		std::unordered_map<std::string, std::string> m_ModulesEntrypoint;

		std::unordered_map<std::string, std::vector<VkDescriptorImageInfo>> m_TextureArrayDescriptors;
		std::unordered_map<std::string, std::vector<VkDescriptorImageInfo>> m_SamplerArrayDescriptors;

		VkDescriptorSetLayout m_RootSignature;
		VkDescriptorPool m_DescriptorPool;
		std::unordered_map<uint32_t, VkDescriptorSet> m_DescriptorSets;
		std::vector<VkDescriptorSet> m_BindableDescriptorSets;

		const VKContext* m_Context;
		VkPipeline m_GraphicsPipeline;
		VkPipelineLayout m_PipelineLayout;
	};
}