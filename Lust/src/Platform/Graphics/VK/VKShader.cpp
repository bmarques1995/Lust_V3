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

Lust::VKShader::VKShader(const std::shared_ptr<VKContext>* context, std::string json_controller_path, InputBufferLayout layout, SmallBufferLayout smallBufferLayout) :
    m_Context(context), m_Layout(layout), m_SmallBufferLayout(smallBufferLayout)
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
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;
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
