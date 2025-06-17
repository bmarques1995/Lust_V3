#include "VKBuffer.hpp"
#include <stdexcept>
#include <cassert>
#include "UniformsLayout.hpp"
#include "VKFunctions.hpp"

VkBuffer Lust::VKBuffer::GetBuffer() const
{
    return m_Buffer;
}

VmaAllocation Lust::VKBuffer::GetAllocation() const
{
    return m_BufferAllocation;
}

Lust::VKBuffer::VKBuffer(const VKContext* context) :
    m_Context(context), m_GPUData(nullptr)
{
}

void Lust::VKBuffer::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VmaAllocation& bufferAllocation, bool dynamicbuffer)
{
    m_IsDynamic = dynamicbuffer;
    VkResult vkr;
    auto device = m_Context->GetDevice();
	auto allocator = m_Context->GetAllocator();

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    // Define allocation info
    VmaAllocationCreateInfo allocCreateInfo = {};
    allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO; // Automatically select memory type
    allocCreateInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT; // Optional: Use dedicated memory

    vkr = VKFunctions::vmaCreateBufferFn(allocator, &bufferInfo, &allocCreateInfo, &buffer, &bufferAllocation, nullptr);
    assert(vkr == VK_SUCCESS);

    if (m_IsDynamic)
    {
        vkr = VKFunctions::vmaMapMemoryFn(allocator, m_BufferAllocation, (void**)&m_GPUData);
        assert(vkr == VK_SUCCESS);
    }
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
    VKFunctions::vkAllocateCommandBuffersFn(device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    VKFunctions::vkBeginCommandBufferFn(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    VKFunctions::vkCmdCopyBufferFn(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    VKFunctions::vkEndCommandBufferFn(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    VKFunctions::vkQueueSubmitFn(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    VKFunctions::vkQueueWaitIdleFn(graphicsQueue);

    VKFunctions::vkFreeCommandBuffersFn(device, commandPool, 1, &commandBuffer);
}

void Lust::VKBuffer::RemapBuffer(const void* data, size_t size, size_t offset)
{
    memcpy(m_GPUData + offset, data, size);
}

void Lust::VKBuffer::RemapBufferStaticly(const void* data, size_t size, size_t offset)
{
    VkResult vkr;
	auto allocator = m_Context->GetAllocator();
    vkr = VKFunctions::vmaMapMemoryFn(allocator, m_BufferAllocation, (void**)&m_GPUData);
    assert(vkr == VK_SUCCESS);
    memcpy(m_GPUData + offset, data, size);
    VKFunctions::vmaUnmapMemoryFn(allocator, m_BufferAllocation);
}

void Lust::VKBuffer::RemapCall(const void* data, size_t size, size_t offset)
{
    if (m_IsDynamic)
        RemapBuffer(data, size, offset);
    else
        RemapBufferStaticly(data, size, offset);
}

uint32_t Lust::VKBuffer::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    auto adapter = m_Context->GetAdapter();
    VkPhysicalDeviceMemoryProperties memProperties;
    VKFunctions::vkGetPhysicalDeviceMemoryPropertiesFn(adapter, &memProperties);

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
	auto allocator = m_Context->GetAllocator();
    if(m_IsDynamic)
        VKFunctions::vmaUnmapMemoryFn(allocator, m_BufferAllocation);
    VKFunctions::vkDeviceWaitIdleFn(device);
    VKFunctions::vmaDestroyBufferFn(allocator, m_Buffer, m_BufferAllocation);
}

Lust::VKVertexBuffer::VKVertexBuffer(const VKContext* context, const void* data, size_t size, uint32_t stride, bool dynamicBuffer) :
    VKBuffer(context)
{
    m_Stride = stride;
    CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        m_Buffer, m_BufferAllocation, dynamicBuffer);
    RemapCall(data, size, 0);
}

Lust::VKVertexBuffer::~VKVertexBuffer()
{
    DestroyBuffer();
}

void Lust::VKVertexBuffer::Stage() const
{
    auto commandBuffer = m_Context->GetCurrentCommandBuffer();
    VkDeviceSize offset = 0;
    VKFunctions::vkCmdBindVertexBuffersFn(commandBuffer, 0, 1, &m_Buffer, &offset);
}

void Lust::VKVertexBuffer::Remap(const void* data, size_t size)
{
	RemapCall(data, size, 0);
}

Lust::VKIndexBuffer::VKIndexBuffer(const VKContext* context, const void* data, uint32_t count, bool dynamicBuffer) :
    VKBuffer(context)
{
    m_Count = (uint32_t)count;
    VkDeviceSize bufferSize = sizeof(uint32_t) * m_Count;
    CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        m_Buffer, m_BufferAllocation, dynamicBuffer);
	RemapCall(data, bufferSize, 0);
}

Lust::VKIndexBuffer::~VKIndexBuffer()
{
    DestroyBuffer();
}

void Lust::VKIndexBuffer::Stage() const
{
    auto commandBuffer = m_Context->GetCurrentCommandBuffer();
    VKFunctions::vkCmdBindIndexBufferFn(commandBuffer, m_Buffer, 0, VK_INDEX_TYPE_UINT32);
}

uint32_t Lust::VKIndexBuffer::GetCount() const
{
	return m_Count;
}

void Lust::VKIndexBuffer::Remap(const void* data, uint32_t count)
{
	RemapCall(data, sizeof(uint32_t) * count, 0);
}

Lust::VKUniformBuffer::VKUniformBuffer(const VKContext* context, const void* data, size_t size) :
    VKBuffer(context)
{
    m_BufferSize = size;
    if (!IsBufferConformed(size))
        throw AttachmentMismatchException(size, m_Context->GetUniformAttachment());
    CreateBuffer(size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_Buffer, m_BufferAllocation);
	Remap(data, size);
}

Lust::VKUniformBuffer::~VKUniformBuffer()
{
    DestroyBuffer();
}

void Lust::VKUniformBuffer::Remap(const void* data, size_t size)
{
    RemapCall(data, size, 0);
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
    CreateBuffer(m_BufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_Buffer, m_BufferAllocation, true);
	Remap(data, size, 0);
}

Lust::VKStructuredBuffer::~VKStructuredBuffer()
{
    DestroyBuffer();
}

void Lust::VKStructuredBuffer::Remap(const void* data, size_t size, size_t offset)
{
    RemapCall(data, size, offset);
}

size_t Lust::VKStructuredBuffer::GetSize() const
{
    return m_BufferSize;
}
