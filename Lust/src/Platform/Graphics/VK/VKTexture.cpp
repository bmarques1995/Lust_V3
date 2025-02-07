#include "VKTexture.hpp"
#include <cassert>
#include "VKCopyPipeline.hpp"
#include "Application.hpp"

#include <vulkan/vulkan.hpp>

Lust::VKTexture2D::VKTexture2D(const std::shared_ptr<VKContext>* context, const TextureElement& specification) :
    m_Context(context), m_Specification(specification)
{
    m_Loaded = false;
    CreateResource();
    CopyBuffer();
    m_Loaded = true;
    m_Specification.FreeImage();
}

Lust::VKTexture2D::~VKTexture2D()
{
    auto device = (*m_Context)->GetDevice();
    vkDeviceWaitIdle(device);
    vkDestroyImageView(device, m_ResourceView, nullptr);
    vkFreeMemory(device, m_Memory, nullptr);
    vkDestroyImage(device, m_Resource, nullptr);
}

const Lust::TextureElement& Lust::VKTexture2D::GetTextureDescription() const
{
	return m_Specification;
}

uint32_t Lust::VKTexture2D::GetWidth() const
{
	return m_Specification.GetWidth();
}

uint32_t Lust::VKTexture2D::GetHeight() const
{
	return m_Specification.GetHeight();
}

bool Lust::VKTexture2D::IsLoaded() const
{
	return m_Loaded;
}

VkImage Lust::VKTexture2D::GetResource() const
{
	return m_Resource;
}

VkDeviceMemory Lust::VKTexture2D::GetMemory() const
{
	return m_Memory;
}

VkImageView Lust::VKTexture2D::GetView() const
{
	return m_ResourceView;
}

void Lust::VKTexture2D::CreateResource()
{
    VkResult vkr;
    auto device = (*m_Context)->GetDevice();
    VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = GetNativeTensor(m_Specification.GetTensor());
    //params
    imageInfo.extent.width = m_Specification.GetWidth();
    imageInfo.extent.height = m_Specification.GetHeight();
    imageInfo.extent.depth = m_Specification.GetDepth();
    imageInfo.mipLevels = m_Specification.GetMipsLevel();

    imageInfo.arrayLayers = 1;
    imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    vkr = vkCreateImage(device, &imageInfo, nullptr, &m_Resource);
    assert(vkr == VK_SUCCESS);

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device, m_Resource, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

    vkr = vkAllocateMemory(device, &allocInfo, nullptr, &m_Memory);
    assert(vkr == VK_SUCCESS);

    vkr = vkBindImageMemory(device, m_Resource, m_Memory, 0);
    assert(vkr == VK_SUCCESS);

    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = m_Resource;
    viewInfo.viewType = GetNativeTensorView(m_Specification.GetTensor());
    viewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    vkr = vkCreateImageView(device, &viewInfo, nullptr, &m_ResourceView);
    assert(vkr == VK_SUCCESS);
}

void Lust::VKTexture2D::CopyBuffer()
{
    VkResult vkr;
    auto device = (*m_Context)->GetDevice();
    std::shared_ptr<VKCopyPipeline>* copyPipeline = (std::shared_ptr<VKCopyPipeline>*)
        (Application::GetInstance()->GetCopyPipeline());

    auto copyCommandBuffer = (*copyPipeline)->GetCommandBuffer();
    auto copyCommandPool = (*copyPipeline)->GetCommandPool();

    VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    size_t imageSize = (m_Specification.GetWidth() * m_Specification.GetHeight() * m_Specification.GetDepth() * m_Specification.GetChannels());

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = imageSize;
    bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    vkr = vkCreateBuffer(device, &bufferInfo, nullptr, &stagingBuffer);
    assert(vkr == VK_SUCCESS);

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, stagingBuffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

    vkr = vkAllocateMemory(device, &allocInfo, nullptr, &stagingBufferMemory);
    assert(vkr == VK_SUCCESS);

    vkBindBufferMemory(device, stagingBuffer, stagingBufferMemory, 0);

    void* GPUData = nullptr;
    vkr = vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &GPUData);
    assert(vkr == VK_SUCCESS);
    memcpy(GPUData, m_Specification.GetTextureBuffer(), imageSize);
    vkUnmapMemory(device, stagingBufferMemory);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(copyCommandBuffer, &beginInfo);

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = m_Resource;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    VkPipelineStageFlags sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    VkPipelineStageFlags destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;

    vkCmdPipelineBarrier(
        copyCommandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = {
        m_Specification.GetWidth(),
        m_Specification.GetHeight(),
        m_Specification.GetDepth()
    };

    vkCmdCopyBufferToImage(copyCommandBuffer, stagingBuffer, m_Resource, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = m_Resource;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

    vkCmdPipelineBarrier(
        copyCommandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    vkEndCommandBuffer(copyCommandBuffer);

    (*copyPipeline)->Wait();

    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);
}

uint32_t Lust::VKTexture2D::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
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

VkImageType Lust::VKTexture2D::GetNativeTensor(TextureTensor tensor)
{
    switch (tensor)
    {
    case TextureTensor::TENSOR_1:
        return VK_IMAGE_TYPE_1D;
    case TextureTensor::TENSOR_2:
        return VK_IMAGE_TYPE_2D;
    case TextureTensor::TENSOR_3:
        return VK_IMAGE_TYPE_3D;
    default:
        return VK_IMAGE_TYPE_MAX_ENUM;
    }
}

VkImageViewType Lust::VKTexture2D::GetNativeTensorView(TextureTensor tensor)
{
    switch (tensor)
    {
    case TextureTensor::TENSOR_1:
        return VK_IMAGE_VIEW_TYPE_1D;
    case TextureTensor::TENSOR_2:
        return VK_IMAGE_VIEW_TYPE_2D;
    case TextureTensor::TENSOR_3:
        return VK_IMAGE_VIEW_TYPE_3D;
    default:
        return VK_IMAGE_VIEW_TYPE_MAX_ENUM;
    }
}
