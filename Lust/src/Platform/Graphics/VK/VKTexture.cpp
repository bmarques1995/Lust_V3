#include "VKTexture.hpp"
#include <cassert>
#include "VKCopyPipeline.hpp"
#include "Application.hpp"
#include "VKFunctions.hpp"

#include <vulkan/vulkan.hpp>
#include <filesystem>

Lust::VKTexture2D::VKTexture2D(const VKContext* context, const TextureBuffer& specification) :
    Texture2D(specification), m_Context(context), m_Specification(specification)
{
    m_Loaded = false;
    CreateResource();
    CopyBuffer();
    m_Loaded = true;
    m_Specification.FreeImage();
}

Lust::VKTexture2D::~VKTexture2D()
{
    auto device = m_Context->GetDevice();
	auto allocator = m_Context->GetAllocator();
    VKFunctions::vkDeviceWaitIdleFn(device);
    VKFunctions::vkDestroyImageViewFn(device, m_ResourceView, nullptr);
    VKFunctions::vmaDestroyImageFn(allocator, m_Resource, m_Allocation);
}

const Lust::TextureBuffer& Lust::VKTexture2D::GetTextureDescription() const
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

VmaAllocation Lust::VKTexture2D::GetAllocation() const
{
	return m_Allocation;
}

VkImageView Lust::VKTexture2D::GetView() const
{
	return m_ResourceView;
}

void Lust::VKTexture2D::CreateResource()
{
    VkResult vkr;
    auto device = m_Context->GetDevice();
	auto allocator = m_Context->GetAllocator();
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

    VmaAllocationCreateInfo imageAllocCreateInfo = {};
    imageAllocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;

    VmaAllocationInfo depthImageAllocInfo = {};

    vkr = VKFunctions::vmaCreateImageFn(allocator, &imageInfo, &imageAllocCreateInfo, &m_Resource, &m_Allocation, &depthImageAllocInfo);
    (vkr == VK_SUCCESS);

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

    vkr = VKFunctions::vkCreateImageViewFn(device, &viewInfo, nullptr, &m_ResourceView);
    assert(vkr == VK_SUCCESS);
}

void Lust::VKTexture2D::CopyBuffer()
{
    VkResult vkr;
    auto device = m_Context->GetDevice();
	auto allocator = m_Context->GetAllocator();
    std::shared_ptr<VKCopyPipeline>* copyPipeline = (std::shared_ptr<VKCopyPipeline>*)
        TextureLibrary::GetCopyPipeline();

    auto copyCommandBuffer = (*copyPipeline)->GetCommandBuffer();
    auto copyCommandPool = (*copyPipeline)->GetCommandPool();

    VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    VkBuffer stagingBuffer;
    VmaAllocation stagingBufferAllocation;
    size_t imageSize = (m_Specification.GetWidth() * m_Specification.GetHeight() * m_Specification.GetDepth() * m_Specification.GetChannels());

	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = imageSize;
	bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	// Define allocation info
	VmaAllocationCreateInfo allocCreateInfo = {};
	allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO; // Automatically select memory type
	allocCreateInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT; // Optional: Use dedicated memory

	vkr = VKFunctions::vmaCreateBufferFn(allocator, &bufferInfo, &allocCreateInfo, &stagingBuffer, &stagingBufferAllocation, nullptr);
	assert(vkr == VK_SUCCESS);

    void* GPUData = nullptr;
    vkr = VKFunctions::vmaMapMemoryFn(allocator, stagingBufferAllocation, &GPUData);
    assert(vkr == VK_SUCCESS);
    memcpy(GPUData, m_Specification.GetTextureBuffer(), imageSize);
    VKFunctions::vmaUnmapMemoryFn(allocator, stagingBufferAllocation);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    VKFunctions::vkBeginCommandBufferFn(copyCommandBuffer, &beginInfo);

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

    VKFunctions::vkCmdPipelineBarrierFn(
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

    VKFunctions::vkCmdCopyBufferToImageFn(copyCommandBuffer, stagingBuffer, m_Resource, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

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

    VKFunctions::vkCmdPipelineBarrierFn(
        copyCommandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    VKFunctions::vkEndCommandBufferFn(copyCommandBuffer);

    (*copyPipeline)->Wait();

    VKFunctions::vmaDestroyBufferFn(allocator, stagingBuffer, stagingBufferAllocation);
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
