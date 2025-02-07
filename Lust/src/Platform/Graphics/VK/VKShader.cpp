#include "VKShader.hpp"
#include "FileHandler.hpp"
#include "Application.hpp"
#include <filesystem>
#include <climits>

namespace fs = std::filesystem;

const std::list<std::string> Lust::VKShader::s_GraphicsPipelineStages =
{
    "vs",
    "ps"
};

const std::unordered_map<std::string, VkShaderStageFlagBits> Lust::VKShader::s_StageCaster =
{
    {"vs", VK_SHADER_STAGE_VERTEX_BIT},
    {"ps", VK_SHADER_STAGE_FRAGMENT_BIT}
};

const std::unordered_map<uint32_t, VkShaderStageFlagBits> Lust::VKShader::s_EnumStageCaster =
{
    {AllowedStages::VERTEX_STAGE, VK_SHADER_STAGE_VERTEX_BIT},
    {AllowedStages::GEOMETRY_STAGE, VK_SHADER_STAGE_GEOMETRY_BIT},
    {AllowedStages::DOMAIN_STAGE, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT},
    {AllowedStages::HULL_STAGE, VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT},
    {AllowedStages::PIXEL_STAGE, VK_SHADER_STAGE_FRAGMENT_BIT},
    {AllowedStages::MESH_STAGE, VK_SHADER_STAGE_MESH_BIT_EXT},
    {AllowedStages::AMPLIFICATION_STAGE, VK_SHADER_STAGE_TASK_BIT_EXT},
};

Lust::VKShader::VKShader(const std::shared_ptr<VKContext>* context, std::string json_controller_path, InputBufferLayout layout, SmallBufferLayout smallBufferLayout, UniformLayout uniformLayout, TextureLayout textureLayout, SamplerLayout samplerLayout) :
    m_Context(context), m_Layout(layout), m_SmallBufferLayout(smallBufferLayout), m_UniformLayout(uniformLayout), m_TextureLayout(textureLayout), m_SamplerLayout(samplerLayout)
{
    VkResult vkr;
    auto device = (*m_Context)->GetDevice();
    auto renderPass = (*m_Context)->GetRenderPass();

    InitJsonAndPaths(json_controller_path);

    std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

    for (auto it = s_GraphicsPipelineStages.begin(); it != s_GraphicsPipelineStages.end(); it++)
    {
        VkPipelineShaderStageCreateInfo pipelineStage;
        PushShader(*it, &pipelineStage);
        shaderStages.push_back(pipelineStage);
    }

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = m_Layout.GetStride();
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    auto nativeElements = m_Layout.GetElements();
    VkVertexInputAttributeDescription* ied = new VkVertexInputAttributeDescription[nativeElements.size()];

    for (size_t i = 0; i < nativeElements.size(); i++)
    {
        ied[i].binding = 0;
        ied[i].location = i;
        ied[i].format = GetNativeFormat(nativeElements[i].GetType());
        ied[i].offset = nativeElements[i].GetOffset();
    }

    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = m_Layout.GetElements().size();
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = ied;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    VkPipelineViewportStateCreateInfo viewportState{};
    VkPipelineMultisampleStateCreateInfo multisampling{};
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    VkPipelineColorBlendStateCreateInfo colorBlending{};
    VkPipelineDepthStencilStateCreateInfo depthStencil{};

    SetInputAssemblyViewportAndMultisampling(&inputAssembly, &viewportState, &multisampling);
    SetRasterizer(&rasterizer);
    SetBlend(&colorBlendAttachment, &colorBlending);
    SetDepthStencil(&depthStencil);
    CreateDescriptorSetLayout();
    CreateDescriptorPool();

    {
        PreallocatesDescSets();

        auto uniforms = m_UniformLayout.GetElements();

        for (const auto& element : uniforms)
        {

            unsigned char* data = new unsigned char[element.second.GetSize()];
            for (size_t i = 0; i < element.second.GetNumberOfBuffers(); i++)
            {
                PreallocateUniform(data, element.second, i);
            }
            delete[] data;
        }

        auto samplers = m_SamplerLayout.GetElements();

        for (const auto& element : samplers)
        {
            CreateSampler(element.second);
        }

        CreateDescriptorSets();
    }

    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    auto smallBuffers = m_SmallBufferLayout.GetElements();

    VkPushConstantRange* pushConstantRange;
    uint32_t pushConstantOffset = 0;
    uint32_t pushConstantCount = smallBuffers.size();

    pushConstantRange = new VkPushConstantRange[pushConstantCount];

    VkShaderStageFlags stageFlag = 0x0;

    for (auto& i : s_EnumStageCaster)
        if ((i.first & m_SmallBufferLayout.GetStages()) != 0)
            stageFlag |= i.second;

    size_t i = 0;
    for (auto& smallBuffer : smallBuffers)
    {
        pushConstantRange[i].offset = pushConstantOffset;
        pushConstantRange[i].size = smallBuffer.second.GetSize();
        pushConstantRange[i].stageFlags = stageFlag;
        pushConstantOffset += smallBuffer.second.GetSize();
    }

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &m_RootSignature;
    pipelineLayoutInfo.pushConstantRangeCount = pushConstantCount;
    pipelineLayoutInfo.pPushConstantRanges = pushConstantRange;

    vkr = vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &m_PipelineLayout);
    assert(vkr == VK_SUCCESS);

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = (uint32_t)shaderStages.size();
    pipelineInfo.pStages = shaderStages.data();
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.layout = m_PipelineLayout;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    vkr = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_GraphicsPipeline);
    assert(vkr == VK_SUCCESS);

    for (auto it = m_Modules.begin(); it != m_Modules.end(); it++)
    {
        vkDestroyShaderModule(device, it->second, nullptr);
        it->second = nullptr;
    }

    delete[] ied;
    delete[] pushConstantRange;
}

Lust::VKShader::~VKShader()
{
    auto device = (*m_Context)->GetDevice();
    vkDeviceWaitIdle(device);
    for (auto& i : m_Samplers)
    {
        vkDestroySampler(device, i.second, nullptr);
    }

    for (auto& i : m_Uniforms)
    {
        vkDestroyBuffer(device, i.second.Resource, nullptr);
        vkFreeMemory(device, i.second.Memory, nullptr);
    }

    vkDestroyDescriptorPool(device, m_DescriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(device, m_RootSignature, nullptr);
    vkDestroyPipeline(device, m_GraphicsPipeline, nullptr);
    vkDestroyPipelineLayout(device, m_PipelineLayout, nullptr);
}

void Lust::VKShader::Stage()
{
    auto commandBuffer = (*m_Context)->GetCurrentCommandBuffer();
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);
}

uint32_t Lust::VKShader::GetStride() const
{
    return m_Layout.GetStride();
}

uint32_t Lust::VKShader::GetOffset() const
{
    return 0;
}

void Lust::VKShader::UploadTexture2D(const std::shared_ptr<Texture2D>* texture)
{
    CreateTextureDescriptorSet((const std::shared_ptr<VKTexture2D>*) texture);
}

void Lust::VKShader::BindSmallBuffer(const void* data, size_t size, uint32_t bindingSlot)
{
    if (size != m_SmallBufferLayout.GetElement(bindingSlot).GetSize())
        throw SizeMismatchException(size, m_SmallBufferLayout.GetElement(bindingSlot).GetSize());
    VkShaderStageFlags bindingFlag = 0;
    for (auto& enumStage : s_EnumStageCaster)
    {
        auto stages = m_SmallBufferLayout.GetStages();
        if (stages & enumStage.first)
            bindingFlag |= enumStage.second;
    }
    vkCmdPushConstants(
        (*m_Context)->GetCurrentCommandBuffer(),
        m_PipelineLayout,
        bindingFlag,
        m_SmallBufferLayout.GetElement(bindingSlot).GetOffset(), // Offset
        size,
        data
    );
}

void Lust::VKShader::BindDescriptors()
{
    auto commandBuffer = (*m_Context)->GetCurrentCommandBuffer();
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, m_BindableDescriptorSets.size(), m_BindableDescriptorSets.data(), 0, nullptr);
}

void Lust::VKShader::UpdateCBuffer(const void* data, size_t size, uint32_t shaderRegister, uint32_t tableIndex)
{
    if (m_UniformLayout.GetElement(shaderRegister).GetAccessLevel() == AccessLevel::ROOT_BUFFER)
        MapUniform(data, size, shaderRegister, 0);
    else
        MapUniform(data, size, shaderRegister, (tableIndex - shaderRegister));
}

void Lust::VKShader::PreallocatesDescSets()
{
    auto device = (*m_Context)->GetDevice();

    VkResult vkr;
    auto uniforms = m_UniformLayout.GetElements();

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_DescriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &m_RootSignature;

    for (auto& uniformElement : uniforms)
    {
        if (m_DescriptorSets.find(uniformElement.second.GetSpaceSet()) == m_DescriptorSets.end())
        {
            vkr = vkAllocateDescriptorSets(device, &allocInfo, &m_DescriptorSets[uniformElement.second.GetSpaceSet()]);
            assert(vkr == VK_SUCCESS);
        }
    }

    for (auto& descSet : m_DescriptorSets)
    {
        m_BindableDescriptorSets.push_back(descSet.second);
    }
}

void Lust::VKShader::CreateDescriptorSetLayout()
{
    VkResult vkr;
    auto device = (*m_Context)->GetDevice();

    std::vector<VkDescriptorSetLayoutBinding> bindings;

    auto uniformElements = m_UniformLayout.GetElements();

    for (auto& i : uniformElements)
    {
        for (size_t j = 0; j < i.second.GetNumberOfBuffers(); j++)
        {
            VkDescriptorSetLayoutBinding binding{};
            binding.binding = i.second.GetBindingSlot() + j;
            binding.descriptorCount = 1;
            binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            binding.pImmutableSamplers = nullptr;
            VkShaderStageFlags stageFlag = 0x0;

            for (auto& i : s_EnumStageCaster)
                if ((i.first & m_UniformLayout.GetStages()) != 0)
                    stageFlag |= i.second;

            binding.stageFlags = stageFlag;
            bindings.push_back(binding);
        }

    }

    auto textureElements = m_TextureLayout.GetElements();
    for (auto& i : textureElements)
    {
        VkDescriptorSetLayoutBinding binding{};
        binding.binding = i.second.GetBindingSlot();
        binding.descriptorCount = 1;
        binding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        binding.pImmutableSamplers = nullptr;
        VkShaderStageFlags stageFlag = 0x0;

        for (auto& i : s_EnumStageCaster)
            if ((i.first & m_UniformLayout.GetStages()) != 0)
                stageFlag |= i.second;

        binding.stageFlags = stageFlag;
        bindings.push_back(binding);
    }

    auto samplerElements = m_SamplerLayout.GetElements();
    for (auto& i : samplerElements)
    {
        VkDescriptorSetLayoutBinding binding{};
        binding.binding = i.second.GetBindingSlot();
        binding.descriptorCount = 1;
        binding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
        binding.pImmutableSamplers = nullptr;
        VkShaderStageFlags stageFlag = 0x0;

        for (auto& i : s_EnumStageCaster)
            if ((i.first & m_UniformLayout.GetStages()) != 0)
                stageFlag |= i.second;

        binding.stageFlags = stageFlag;
        bindings.push_back(binding);
    }

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    vkr = vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &m_RootSignature);
    assert(vkr == VK_SUCCESS);
}

void Lust::VKShader::CreateDescriptorPool()
{
    VkResult vkr;
    auto device = (*m_Context)->GetDevice();

    std::vector<VkDescriptorPoolSize> poolSize;
    auto uniformElements = m_UniformLayout.GetElements();
    for (auto& i : uniformElements)
    {
        for (size_t j = 0; j < i.second.GetNumberOfBuffers(); j++)
        {
            VkDescriptorPoolSize poolSizer;
            poolSizer.type = GetNativeDescriptorType(i.second.GetBufferType());
            poolSizer.descriptorCount = 1;
            poolSize.push_back(poolSizer);
        }
    }

    auto textureElements = m_TextureLayout.GetElements();
    for (auto& i : textureElements)
    {
        VkDescriptorPoolSize poolSizer;
        poolSizer.type = GetNativeDescriptorType(BufferType::TEXTURE_BUFFER);
        poolSizer.descriptorCount = 1;
        poolSize.push_back(poolSizer);
    }

    auto samplerElements = m_SamplerLayout.GetElements();
    for (auto& i : samplerElements)
    {
        VkDescriptorPoolSize poolSizer;
        poolSizer.type = GetNativeDescriptorType(BufferType::SAMPLER_BUFFER);
        poolSizer.descriptorCount = 1;
        poolSize.push_back(poolSizer);
    }

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = poolSize.data();
    poolInfo.maxSets = poolSize.size();

    vkr = vkCreateDescriptorPool(device, &poolInfo, nullptr, &m_DescriptorPool);
    assert(vkr == VK_SUCCESS);
}

void Lust::VKShader::CreateDescriptorSets()
{
    VkResult vkr;
    auto device = (*m_Context)->GetDevice();

    std::vector<VkWriteDescriptorSet> descriptorWrites;
    std::vector<VkDescriptorImageInfo> samplerInfos;
    std::vector<VkDescriptorBufferInfo> bufferInfos;

    auto uniforms = m_UniformLayout.GetElements();
    auto samplers = m_SamplerLayout.GetElements();

    size_t i = 0;

    VkDescriptorBufferInfo bufferInfo{};
    VkWriteDescriptorSet descriptorWrite{};

    for (auto& uniformElement : uniforms)
    {
        for (size_t j = 0; j < uniformElement.second.GetNumberOfBuffers(); j++)
        {
            bufferInfo = {};
            bufferInfo.buffer = m_Uniforms[uniformElement.second.GetBindingSlot() + j].Resource;
            bufferInfo.offset = 0;
            bufferInfo.range = uniformElement.second.GetSize();

            bufferInfos.push_back(bufferInfo);
        }
    }

    for (auto& samplerElement : samplers)
    {

        //assert(vkr == VK_SUCCESS);

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = nullptr;
        imageInfo.sampler = m_Samplers[samplerElement.second.GetBindingSlot()];

        samplerInfos.push_back(imageInfo);
    }

    for (auto& uniformElement : uniforms)
    {
        for (size_t j = 0; j < uniformElement.second.GetNumberOfBuffers(); j++)
        {
            descriptorWrite = {};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = m_DescriptorSets[uniformElement.second.GetSpaceSet()];
            descriptorWrite.dstBinding = uniformElement.second.GetBindingSlot() + j;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = GetNativeDescriptorType(uniformElement.second.GetBufferType());
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pBufferInfo = &bufferInfos[i];

            descriptorWrites.push_back(descriptorWrite);
            i++;
        }
    }

    i = 0;
    for (auto& samplerElement : samplers)
    {
        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = m_DescriptorSets[samplerElement.second.GetSpaceSet()];
        descriptorWrite.dstBinding = samplerElement.second.GetBindingSlot();
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = GetNativeDescriptorType(BufferType::SAMPLER_BUFFER);
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pImageInfo = &samplerInfos[i];

        descriptorWrites.push_back(descriptorWrite);

        i++;
    }

    vkUpdateDescriptorSets(device, descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
    descriptorWrites.clear();
}

void Lust::VKShader::CreateTextureDescriptorSet(const std::shared_ptr<VKTexture2D>* texture)
{
    VkResult vkr;
    auto device = (*m_Context)->GetDevice();

    VkWriteDescriptorSet descriptorWrite{};
    VkDescriptorImageInfo imageInfo{};

    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = (*texture)->GetView();
    imageInfo.sampler = nullptr;

    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = m_DescriptorSets[(*texture)->GetTextureDescription().GetSpaceSet()];
    descriptorWrite.dstBinding = (*texture)->GetTextureDescription().GetBindingSlot();
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = GetNativeDescriptorType(BufferType::TEXTURE_BUFFER);
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
}

bool Lust::VKShader::IsUniformValid(size_t size)
{
    return ((size % (*m_Context)->GetUniformAttachment()) == 0);
}

void Lust::VKShader::PreallocateUniform(const void* data, UniformElement uniformElement, uint32_t offset)
{
    uint32_t bindingPoint = uniformElement.GetBindingSlot() + offset;
    if (!IsUniformValid(uniformElement.GetSize()))
        throw AttachmentMismatchException(uniformElement.GetSize(), (*m_Context)->GetUniformAttachment());

    VkResult vkr;
    auto device = (*m_Context)->GetDevice();
    VkDeviceSize bufferSize = uniformElement.GetSize();
    m_Uniforms[bindingPoint] = {};

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = uniformElement.GetSize();
    bufferInfo.usage = GetNativeBufferUsage(uniformElement.GetBufferType());
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    vkr = vkCreateBuffer(device, &bufferInfo, nullptr, &m_Uniforms[bindingPoint].Resource);
    assert(vkr == VK_SUCCESS);

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, m_Uniforms[bindingPoint].Resource, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    vkr = vkAllocateMemory(device, &allocInfo, nullptr, &m_Uniforms[bindingPoint].Memory);
    assert(vkr == VK_SUCCESS);

    vkBindBufferMemory(device, m_Uniforms[bindingPoint].Resource, m_Uniforms[bindingPoint].Memory, 0);

    MapUniform(data, uniformElement.GetSize(), uniformElement.GetBindingSlot(), offset);

}

void Lust::VKShader::MapUniform(const void* data, size_t size, uint32_t shaderRegister, uint32_t offset)
{
    VkResult vkr;
    void* gpuData;
    auto device = (*m_Context)->GetDevice();
    vkr = vkMapMemory(device, m_Uniforms[shaderRegister + offset].Memory, 0, size, 0, &gpuData);
    assert(vkr == VK_SUCCESS);
    memcpy(gpuData, data, size);
    vkUnmapMemory(device, m_Uniforms[shaderRegister + offset].Memory);
}

uint32_t Lust::VKShader::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    auto adapter = (*m_Context)->GetAdapter();
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(adapter, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    return 0xffffffff;
}

void Lust::VKShader::CreateSampler(SamplerElement samplerElement)
{
    VkResult vkr;
    auto device = (*m_Context)->GetDevice();
    auto adapter = (*m_Context)->GetAdapter();

    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(adapter, &properties);

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = GetNativeFilter(samplerElement.GetFilter());
    samplerInfo.minFilter = GetNativeFilter(samplerElement.GetFilter());
    samplerInfo.addressModeU = GetNativeAddressMode(samplerElement.GetAddressMode());
    samplerInfo.addressModeV = GetNativeAddressMode(samplerElement.GetAddressMode());
    samplerInfo.addressModeW = GetNativeAddressMode(samplerElement.GetAddressMode());
    samplerInfo.anisotropyEnable = samplerElement.GetFilter() == SamplerFilter::ANISOTROPIC ? VK_TRUE : VK_FALSE;
    samplerInfo.maxAnisotropy = std::min<float>(properties.limits.maxSamplerAnisotropy, (1 << (uint32_t)samplerElement.GetAnisotropicFactor()) * 1.0f);
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_TRUE;
    samplerInfo.compareOp = (VkCompareOp)((uint32_t)samplerElement.GetComparisonPassMode());
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = FLT_MAX;

    vkr = vkCreateSampler(device, &samplerInfo, nullptr, &m_Samplers[samplerElement.GetBindingSlot()]);
    assert(vkr == VK_SUCCESS);
}

void Lust::VKShader::PushShader(std::string_view stage, VkPipelineShaderStageCreateInfo* graphicsDesc)
{
    VkShaderStageFlagBits stageEnum;
    auto it = s_StageCaster.find(stage.data());
    if (it != s_StageCaster.end())
        stageEnum = it->second;
    else
        return;
    if (m_Modules[stage.data()] != nullptr)
        return;
    VkResult vkr;
    auto device = (*m_Context)->GetDevice();

    std::string shaderName = m_PipelineInfo["BinShaders"][stage.data()]["filename"].asString();
    std::stringstream shaderFullPath;
    shaderFullPath << m_ShaderDir << "/" << shaderName;
    std::string shaderPath = shaderFullPath.str();
    m_ModulesEntrypoint[stage.data()] = m_PipelineInfo["BinShaders"][stage.data()]["entrypoint"].asString();

    if (!FileHandler::FileExists(shaderPath))
        return;

    size_t blobSize;
    std::byte* blobData;

    if (!FileHandler::ReadBinFile(shaderPath, &blobData, &blobSize))
        return;

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = blobSize;
    createInfo.pCode = reinterpret_cast<const uint32_t*>(blobData);

    vkr = vkCreateShaderModule(device, &createInfo, nullptr, &m_Modules[stage.data()]);
    assert(vkr == VK_SUCCESS);

    memset(graphicsDesc, 0, sizeof(VkPipelineShaderStageCreateInfo));
    graphicsDesc->sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    graphicsDesc->stage = stageEnum;
    graphicsDesc->module = m_Modules[stage.data()];
    graphicsDesc->pName = m_ModulesEntrypoint[stage.data()].c_str();

    delete[] blobData;
}

void Lust::VKShader::InitJsonAndPaths(std::string json_controller_path)
{
    Json::Reader reader;
    std::string jsonResult;
    FileHandler::ReadTextFile(json_controller_path, &jsonResult);
    reader.parse(jsonResult, m_PipelineInfo);

    fs::path location = json_controller_path;
    m_ShaderDir = location.parent_path().string();
}

void Lust::VKShader::SetRasterizer(VkPipelineRasterizationStateCreateInfo* rasterizer)
{
    rasterizer->sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer->depthClampEnable = VK_FALSE;
    rasterizer->rasterizerDiscardEnable = VK_FALSE;
    rasterizer->polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer->lineWidth = 1.0f;
    rasterizer->cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer->frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer->depthBiasEnable = VK_FALSE;
}

void Lust::VKShader::SetInputAssemblyViewportAndMultisampling(VkPipelineInputAssemblyStateCreateInfo* inputAssembly, VkPipelineViewportStateCreateInfo* viewportState, VkPipelineMultisampleStateCreateInfo* multisampling)
{
    inputAssembly->sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly->topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly->primitiveRestartEnable = VK_FALSE;

    viewportState->sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState->viewportCount = 1;
    viewportState->scissorCount = 1;

    multisampling->sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling->sampleShadingEnable = VK_FALSE;
    multisampling->rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
}

void Lust::VKShader::SetBlend(VkPipelineColorBlendAttachmentState* colorBlendAttachment, VkPipelineColorBlendStateCreateInfo* colorBlending)
{
    colorBlendAttachment->blendEnable = VK_TRUE; // Enable blending
    colorBlendAttachment->srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA; // Source alpha
    colorBlendAttachment->dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA; // One minus source alpha
    colorBlendAttachment->colorBlendOp = VK_BLEND_OP_ADD; // Add color components
    colorBlendAttachment->srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Source alpha for alpha blending
    colorBlendAttachment->dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA; // One minus source alpha for alpha blending
    colorBlendAttachment->alphaBlendOp = VK_BLEND_OP_ADD; // Add alpha components
    colorBlendAttachment->colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT; // Write all components

    colorBlending->sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending->logicOpEnable = VK_FALSE; // Disable logical operations
    colorBlending->attachmentCount = 1; // One color attachment
    colorBlending->pAttachments = colorBlendAttachment;
    colorBlending->logicOp = VK_LOGIC_OP_COPY; // Irrelevant when logicOpEnable is VK_FALSE
    colorBlending->blendConstants[0] = 0.0f; // Blend constants (not used here)
    colorBlending->blendConstants[1] = 0.0f;
    colorBlending->blendConstants[2] = 0.0f;
    colorBlending->blendConstants[3] = 0.0f;
}

void Lust::VKShader::SetDepthStencil(VkPipelineDepthStencilStateCreateInfo* depthStencil)
{
    depthStencil->sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil->depthTestEnable = VK_TRUE;
    depthStencil->depthWriteEnable = VK_TRUE;
    depthStencil->depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    depthStencil->depthBoundsTestEnable = VK_FALSE;
    depthStencil->stencilTestEnable = VK_FALSE;
}

VkFormat Lust::VKShader::GetNativeFormat(ShaderDataType type)
{
    switch (type)
    {
    case ShaderDataType::Float: return VK_FORMAT_R32_SFLOAT;
    case ShaderDataType::Float2: return VK_FORMAT_R32G32_SFLOAT;
    case ShaderDataType::Float3: return VK_FORMAT_R32G32B32_SFLOAT;
    case ShaderDataType::Float4: return VK_FORMAT_R32G32B32A32_SFLOAT;
    case ShaderDataType::Uint: return VK_FORMAT_R32_UINT;
    case ShaderDataType::Uint2: return VK_FORMAT_R32G32_UINT;
    case ShaderDataType::Uint3: return VK_FORMAT_R32G32B32_UINT;
    case ShaderDataType::Uint4: return VK_FORMAT_R32G32B32A32_UINT;
    case ShaderDataType::Bool: return VK_FORMAT_R8_UINT;
    default: return VK_FORMAT_UNDEFINED;
    }
}

VkDescriptorType Lust::VKShader::GetNativeDescriptorType(BufferType type)
{
    switch (type)
    {
    case BufferType::UNIFORM_CONSTANT_BUFFER:
        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    case BufferType::TEXTURE_BUFFER:
        return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    case BufferType::SAMPLER_BUFFER:
        return VK_DESCRIPTOR_TYPE_SAMPLER;
    default:
    case BufferType::INVALID_BUFFER_TYPE:
        return VK_DESCRIPTOR_TYPE_MAX_ENUM;
    }
}

VkBufferUsageFlagBits Lust::VKShader::GetNativeBufferUsage(BufferType type)
{
    switch (type)
    {
    case BufferType::UNIFORM_CONSTANT_BUFFER:
        return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    default:
    case BufferType::INVALID_BUFFER_TYPE:
        return VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM;
    }
}

VkFilter Lust::VKShader::GetNativeFilter(SamplerFilter filter)
{
    switch (filter)
    {
    case SamplerFilter::ANISOTROPIC:
    case SamplerFilter::LINEAR:
        return VK_FILTER_LINEAR;
    case SamplerFilter::NEAREST:
        return VK_FILTER_NEAREST;
    default:
        return VK_FILTER_MAX_ENUM;
    }
}

VkSamplerAddressMode Lust::VKShader::GetNativeAddressMode(AddressMode addressMode)
{
    switch (addressMode)
    {
    case AddressMode::REPEAT:
        return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    case AddressMode::MIRROR:
        return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    case AddressMode::CLAMP:
        return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    case AddressMode::BORDER:
        return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    case AddressMode::MIRROR_ONCE:
        return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
    default:
        return VK_SAMPLER_ADDRESS_MODE_MAX_ENUM;
    }
}
