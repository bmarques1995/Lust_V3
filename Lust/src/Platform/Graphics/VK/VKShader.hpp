#pragma once

#include "Shader.hpp"
#include "VKContext.hpp"
#include "VKTexture.hpp"
#include "DXCSafeInclude.hpp"
#include <json/json.h>
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
		VKShader(const std::shared_ptr<VKContext>* context, std::string json_controller_path, InputBufferLayout layout, SmallBufferLayout smallBufferLayout, UniformLayout uniformLayout, TextureLayout textureLayout, SamplerLayout samplerLayout);
		~VKShader();

		void Stage() override;
		uint32_t GetStride() const override;
		uint32_t GetOffset() const override;

		void UploadTexture2D(const std::shared_ptr<Texture2D>* texture) override;

		void BindSmallBuffer(const void* data, size_t size, uint32_t bindingSlot) override;

		void BindDescriptors() override;

		void UpdateCBuffer(const void* data, size_t size, uint32_t shaderRegister, uint32_t tableIndex) override;

	private:

		void PreallocatesDescSets();

		void CreateDescriptorSetLayout();
		void CreateDescriptorPool();
		void CreateDescriptorSets();

		void CreateTextureDescriptorSet(const std::shared_ptr<VKTexture2D>* texture);

		bool IsUniformValid(size_t size);
		void PreallocateUniform(const void* data, UniformElement uniformElement, uint32_t offset);
		void MapUniform(const void* data, size_t size, uint32_t shaderRegister, uint32_t offset);
		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

		void CreateSampler(SamplerElement samplerElement);

		void PushShader(std::string_view stage, VkPipelineShaderStageCreateInfo* graphicsDesc);
		void InitJsonAndPaths(std::string json_controller_path);
		void SetRasterizer(VkPipelineRasterizationStateCreateInfo* rasterizer);
		void SetInputAssemblyViewportAndMultisampling(VkPipelineInputAssemblyStateCreateInfo* inputAssembly, VkPipelineViewportStateCreateInfo* viewportState, VkPipelineMultisampleStateCreateInfo* multisampling);
		void SetBlend(VkPipelineColorBlendAttachmentState* colorBlendAttachment, VkPipelineColorBlendStateCreateInfo* colorBlending);
		void SetDepthStencil(VkPipelineDepthStencilStateCreateInfo* depthStencil);

		static VkFormat GetNativeFormat(ShaderDataType type);
		static VkDescriptorType GetNativeDescriptorType(BufferType type);
		static VkBufferUsageFlagBits GetNativeBufferUsage(BufferType type);
		static VkFilter GetNativeFilter(SamplerFilter filter);
		static VkSamplerAddressMode GetNativeAddressMode(AddressMode addressMode);


		static const std::list<std::string> s_GraphicsPipelineStages;

		static const std::unordered_map<std::string, VkShaderStageFlagBits> s_StageCaster;
		static const std::unordered_map<uint32_t, VkShaderStageFlagBits> s_EnumStageCaster;

		std::unordered_map<std::string, VkShaderModule> m_Modules;
		std::unordered_map<std::string, std::string> m_ModulesEntrypoint;

		Json::Value m_PipelineInfo;

		std::unordered_map<uint32_t, RM> m_Uniforms;
		std::unordered_map<uint32_t, VkSampler> m_Samplers;

		InputBufferLayout m_Layout;
		SmallBufferLayout m_SmallBufferLayout;
		UniformLayout m_UniformLayout;
		TextureLayout m_TextureLayout;
		SamplerLayout m_SamplerLayout;

		VkDescriptorSetLayout m_RootSignature;
		VkDescriptorPool m_DescriptorPool;
		std::unordered_map<uint32_t, VkDescriptorSet> m_DescriptorSets;
		std::vector<VkDescriptorSet> m_BindableDescriptorSets;

		const std::shared_ptr<VKContext>* m_Context;
		std::string m_ShaderDir;
		VkPipeline m_GraphicsPipeline;
		VkPipelineLayout m_PipelineLayout;
	};
}