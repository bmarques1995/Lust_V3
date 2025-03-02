#include "VKBuffer.hpp"
#include <stdexcept>
#include <cassert>
#include "UniformsLayout.hpp"

VkBuffer Lust::VKBuffer::GetBuffer() const
{
    return m_Buffer;
}

VkDeviceMemory Lust::VKBuffer::GetMemory() const
{
    return m_BufferMemory;
}

Lust::VKBuffer::VKBuffer(const VKContext* context) :
    m_Context(context)
{
}

void Lust::VKBuffer::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
    VkResult vkr;
    auto device = m_Context->GetDevice();
    
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    vkr = vkCreateBuffer(device, &bufferInfo, nullptr, &buffer);
    assert(vkr == VK_SUCCESS);

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

    vkr = vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory);
    assert(vkr == VK_SUCCESS);
    vkBindBufferMemory(device, buffer, bufferMemory, 0);
}

void Lust::VKBuffer::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
    auto commandPool = m_Context->GetCommandPool();
    auto device = m_Context->GetDevice();
    auto graphicsQueue = m_Context->GetGraphicsQueue();

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);

    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

void Lust::VKBuffer::RemapBuffer(const void* data, size_t size, size_t offset)
{
    VkResult vkr;
    auto device = m_Context->GetDevice();
    uint8_t* gpuData;
    vkr = vkMapMemory(device, m_BufferMemory, 0, size, 0, (void**)&gpuData);
    assert(vkr == VK_SUCCESS);
    memcpy(gpuData + offset, data, size);
    vkUnmapMemory(device, m_BufferMemory);
}

uint32_t Lust::VKBuffer::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    auto adapter = m_Context->GetAdapter();
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(adapter, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    assert(false);
    return 0xffffffffu;
}

bool Lust::VKBuffer::IsBufferConformed(size_t size)
{
    return ((size % m_Context->GetUniformAttachment()) == 0);
}

void Lust::VKBuffer::DestroyBuffer()
{
    auto device = m_Context->GetDevice();
    vkDeviceWaitIdle(device);
    vkDestroyBuffer(device, m_Buffer, nullptr);
    vkFreeMemory(device, m_BufferMemory, nullptr);
}

Lust::VKVertexBuffer::VKVertexBuffer(const VKContext* context, const void* data, size_t size, uint32_t stride) :
    VKBuffer(context)
{
    m_Stride = stride;
    CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Buffer, m_BufferMemory);
    RemapBuffer(data, size, 0);
}

Lust::VKVertexBuffer::~VKVertexBuffer()
{
    DestroyBuffer();
}

void Lust::VKVertexBuffer::Stage() const
{
    auto commandBuffer = m_Context->GetCurrentCommandBuffer();
    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &m_Buffer, &offset);
}

Lust::VKIndexBuffer::VKIndexBuffer(const VKContext* context, const void* data, size_t count) :
    VKBuffer(context)
{
    m_Count = (uint32_t)count;
    VkDeviceSize bufferSize = sizeof(uint32_t) * m_Count;
    CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Buffer, m_BufferMemory);
	RemapBuffer(data, bufferSize, 0);
}

Lust::VKIndexBuffer::~VKIndexBuffer()
{
    DestroyBuffer();
}

void Lust::VKIndexBuffer::Stage() const
{
    auto commandBuffer = m_Context->GetCurrentCommandBuffer();
    vkCmdBindIndexBuffer(commandBuffer, m_Buffer, 0, VK_INDEX_TYPE_UINT32);
}

uint32_t Lust::VKIndexBuffer::GetCount() const
{
	return m_Count;
}

Lust::VKUniformBuffer::VKUniformBuffer(const VKContext* context, const void* data, size_t size) :
    VKBuffer(context)
{
    m_BufferSize = size;
    if (!IsBufferConformed(size))
        throw AttachmentMismatchException(size, m_Context->GetUniformAttachment());
    CreateBuffer(size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_Buffer, m_BufferMemory);
	Remap(data, size);
}

Lust::VKUniformBuffer::~VKUniformBuffer()
{
    DestroyBuffer();
}

void Lust::VKUniformBuffer::Remap(const void* data, size_t size)
{
    RemapBuffer(data, size, 0);
}

size_t Lust::VKUniformBuffer::GetSize() const
{
    return m_BufferSize;
}

Lust::VKStructuredBuffer::VKStructuredBuffer(const VKContext* context, const void* data, size_t size) :
    VKBuffer(context)
{
    m_BufferSize = size;
    if (!IsBufferConformed(size))
    {
        size_t bufferCorrection = size;
        bufferCorrection += (m_Context->GetUniformAttachment() - (bufferCorrection % m_Context->GetUniformAttachment()));
        m_BufferSize += bufferCorrection;
    }
    CreateBuffer(m_BufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_Buffer, m_BufferMemory);
	Remap(data, size, 0);
}

Lust::VKStructuredBuffer::~VKStructuredBuffer()
{
    DestroyBuffer();
}

void Lust::VKStructuredBuffer::Remap(const void* data, size_t size, size_t offset)
{
    RemapBuffer(data, size, offset);
}

size_t Lust::VKStructuredBuffer::GetSize() const
{
    return m_BufferSize;
}
