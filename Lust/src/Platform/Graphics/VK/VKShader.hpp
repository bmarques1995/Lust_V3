#pragma once

#include "Shader.hpp"
#include "VKContext.hpp"
#include "VKTexture.hpp"
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
		VKShader(const VKContext* context, std::string json_controller_path, InputInfo inputInfo);
		~VKShader();

		void Stage() override;
		uint32_t GetStride() const override;
		uint32_t GetOffset() const override;

		void UploadTexture2D(const std::shared_ptr<Texture2D>* texture, const TextureElement& textureElement) override;

		void UploadConstantBuffer(const std::shared_ptr<UniformBuffer>* buffer, const UniformElement& uploadCBV) override;

		void UploadStructuredBuffer(const std::shared_ptr<StructuredBuffer>* buffer, const StructuredBufferElement& uploadSRV) override;

		void BindSmallBuffer(const void* data, size_t size, const SmallBufferElement& smallBuffer, size_t offset) override;

		void BindDescriptors() override;

		void CreateSampler(const SamplerElement& samplerElement, const SamplerInfo& info) override;

	private:

		void PreallocatesDescSets();

		void CreateDescriptorSetLayout();
		void CreateDescriptorPool();
		void CreateDescriptorSets();

		void CreateBuffer(size_t bufferSize, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer* buffer, VkDeviceMemory* bufferMemory);

		void CreateTextureDescriptorSet(const std::shared_ptr<VKTexture2D>* texture, const TextureElement& textureElement);
		void CreateUniformDescriptorSet(const std::shared_ptr<VKUniformBuffer>* buffer, const UniformElement& uniformElement);
		void CreateStructuredBufferDescriptorSet(const std::shared_ptr<VKStructuredBuffer>* buffer, const StructuredBufferElement& uniformElement);

		void UpdateSamplerDescriptorSet(const SamplerElement& samplerElement);

		bool IsUniformValid(size_t size);

		//void PreallocateSSBO(const StructuredBufferElement& structuredBufferElement, uint32_t offset);
		//void MapSSBO(const void* data, size_t size, uint32_t shaderRegister, uint32_t offset);
		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

		void PushShader(std::string_view stage, VkPipelineShaderStageCreateInfo* graphicsDesc);
		void SetRasterizer(VkPipelineRasterizationStateCreateInfo* rasterizer);
		void SetInputAssemblyViewportAndMultisampling(VkPipelineInputAssemblyStateCreateInfo* inputAssembly, VkPipelineViewportStateCreateInfo* viewportState, VkPipelineMultisampleStateCreateInfo* multisampling, Topology topology);
		void SetBlend(VkPipelineColorBlendAttachmentState* colorBlendAttachment, VkPipelineColorBlendStateCreateInfo* colorBlending);
		void SetDepthStencil(VkPipelineDepthStencilStateCreateInfo* depthStencil);

		static VkFormat GetNativeFormat(ShaderDataType type);
		static VkDescriptorType GetNativeDescriptorType(BufferType type);
		static VkBufferUsageFlagBits GetNativeBufferUsage(BufferType type);
		static VkFilter GetNativeFilter(SamplerFilter filter);
		static VkSamplerAddressMode GetNativeAddressMode(AddressMode addressMode);
		static VkPrimitiveTopology GetNativeTopology(Topology topology);

		static const std::list<std::string> s_GraphicsPipelineStages;

		static const std::unordered_map<std::string, VkShaderStageFlagBits> s_StageCaster;
		static const std::unordered_map<uint32_t, VkShaderStageFlagBits> s_EnumStageCaster;

		std::unordered_map<std::string, VkShaderModule> m_Modules;
		std::unordered_map<std::string, std::string> m_ModulesEntrypoint;

		//std::unordered_map<uint32_t, RM> m_Uniforms;
		//std::unordered_map<uint32_t, RM> m_SSBOs;
		std::unordered_map<uint32_t, VkSampler> m_Samplers;

		VkDescriptorSetLayout m_RootSignature;
		VkDescriptorPool m_DescriptorPool;
		std::unordered_map<uint32_t, VkDescriptorSet> m_DescriptorSets;
		std::vector<VkDescriptorSet> m_BindableDescriptorSets;

		const VKContext* m_Context;
		VkPipeline m_GraphicsPipeline;
		VkPipelineLayout m_PipelineLayout;
	};
}