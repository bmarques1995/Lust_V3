#include "VKCopyPipeline.hpp"
#include <cassert>
#include "VKFunctions.hpp"

Lust::VKCopyPipeline::VKCopyPipeline(const VKContext* context) :
    m_Context(context)
{
    auto device = m_Context->GetDevice();
    auto adapter = m_Context->GetAdapter();
    VkResult vkr;

    QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(adapter);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    vkr = VKFunctions::vkCreateCommandPoolFn(device, &poolInfo, nullptr, &m_CopyCommandPool);
    assert(vkr == VK_SUCCESS);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_CopyCommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    vkr = VKFunctions::vkAllocateCommandBuffersFn(device, &allocInfo, &m_CopyCommandBuffer);
    assert(vkr == VK_SUCCESS);

    VKFunctions::vkGetDeviceQueueFn(device, 0, 0, &m_CopyQueue);
}

Lust::VKCopyPipeline::~VKCopyPipeline()
{
    auto device = m_Context->GetDevice();
    VKFunctions::vkDeviceWaitIdleFn(device);

    VKFunctions::vkFreeCommandBuffersFn(device, m_CopyCommandPool, 1, &m_CopyCommandBuffer);
    VKFunctions::vkDestroyCommandPoolFn(device, m_CopyCommandPool, nullptr);
}

void Lust::VKCopyPipeline::Wait()
{
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_CopyCommandBuffer;

    VKFunctions::vkQueueSubmitFn(m_CopyQueue, 1, &submitInfo, VK_NULL_HANDLE);
    VKFunctions::vkQueueWaitIdleFn(m_CopyQueue);
}

VkQueue Lust::VKCopyPipeline::GetCopyQueue() const
{
    return m_CopyQueue;
}

VkCommandBuffer Lust::VKCopyPipeline::GetCommandBuffer() const
{
    return m_CopyCommandBuffer;
}

VkCommandPool Lust::VKCopyPipeline::GetCommandPool() const
{
    return m_CopyCommandPool;
}

Lust::QueueFamilyIndices Lust::VKCopyPipeline::FindQueueFamilies(VkPhysicalDevice adapter)
{
    auto surface = m_Context->GetSurface();
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    VKFunctions::vkGetPhysicalDeviceQueueFamilyPropertiesFn(adapter, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    VKFunctions::vkGetPhysicalDeviceQueueFamilyPropertiesFn(adapter, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies)
    {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            indices.graphicsFamily = i;
        VkBool32 presentSupport = false;
        VKFunctions::vkGetPhysicalDeviceSurfaceSupportKHRFn(adapter, i, surface, &presentSupport);
        if (presentSupport)
            indices.presentFamily = i;

        if (indices.isComplete())
            break;
        i++;
    }

    return indices;
}
