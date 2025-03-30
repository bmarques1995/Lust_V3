#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

namespace Lust 
{
    class VKFunctions
    {
    public:
        static void LoadVulkanFunctions();
        static void UnloadVulkanFunctions();

        static void LoadMockVulkanFunctions();

        static bool IsLoaded();

#ifdef LUST_USES_WINDOWS
		static PFN_vkCreateWin32SurfaceKHR vkCreateWin32SurfaceKHRFn;
#endif

        static PFN_vkCmdDrawIndexed vkCmdDrawIndexedFn;
        static PFN_vkCmdDraw vkCmdDrawFn;
        static PFN_vkDestroySurfaceKHR vkDestroySurfaceKHRFn;
        static PFN_vkCreateBuffer vkCreateBufferFn;
        static PFN_vkGetBufferMemoryRequirements vkGetBufferMemoryRequirementsFn;
        static PFN_vkAllocateMemory vkAllocateMemoryFn;
        static PFN_vkBindBufferMemory vkBindBufferMemoryFn;
        static PFN_vkAllocateCommandBuffers vkAllocateCommandBuffersFn;
        static PFN_vkBeginCommandBuffer vkBeginCommandBufferFn;
        static PFN_vkCmdCopyBuffer vkCmdCopyBufferFn;
        static PFN_vkEndCommandBuffer vkEndCommandBufferFn;
        static PFN_vkQueueSubmit vkQueueSubmitFn;
        static PFN_vkQueueWaitIdle vkQueueWaitIdleFn;
        static PFN_vkFreeCommandBuffers vkFreeCommandBuffersFn;
        static PFN_vkMapMemory vkMapMemoryFn;
        static PFN_vkUnmapMemory vkUnmapMemoryFn;
        static PFN_vkGetPhysicalDeviceMemoryProperties vkGetPhysicalDeviceMemoryPropertiesFn;
        static PFN_vkDeviceWaitIdle vkDeviceWaitIdleFn;
        static PFN_vkDestroyBuffer vkDestroyBufferFn;
        static PFN_vkFreeMemory vkFreeMemoryFn;
        static PFN_vkCmdBindVertexBuffers vkCmdBindVertexBuffersFn;
        static PFN_vkCmdBindIndexBuffer vkCmdBindIndexBufferFn;
        static PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerPropertiesFn;
        static PFN_vkDestroyFence vkDestroyFenceFn;
        static PFN_vkDestroySemaphore vkDestroySemaphoreFn;
        static PFN_vkDestroyCommandPool vkDestroyCommandPoolFn;
        static PFN_vkDestroyRenderPass vkDestroyRenderPassFn;
        static PFN_vkDestroyDevice vkDestroyDeviceFn;
        static PFN_vkGetInstanceProcAddr vkGetInstanceProcAddrFn;
        static PFN_vkDestroyInstance vkDestroyInstanceFn;
        static PFN_vkCmdBeginRenderPass vkCmdBeginRenderPassFn;
        static PFN_vkCmdEndRenderPass vkCmdEndRenderPassFn;
        static PFN_vkWaitForFences vkWaitForFencesFn;
        static PFN_vkAcquireNextImageKHR vkAcquireNextImageKHRFn;
        static PFN_vkResetFences vkResetFencesFn;
        static PFN_vkResetCommandBuffer vkResetCommandBufferFn;
        static PFN_vkQueuePresentKHR vkQueuePresentKHRFn;
        static PFN_vkCmdSetViewport vkCmdSetViewportFn;
        static PFN_vkCmdSetScissor vkCmdSetScissorFn;
        static PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDevicePropertiesFn;
        static PFN_vkCreateInstance vkCreateInstanceFn;
        static PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevicesFn;
        static PFN_vkGetPhysicalDeviceQueueFamilyProperties vkGetPhysicalDeviceQueueFamilyPropertiesFn;
        static PFN_vkGetPhysicalDeviceSurfaceSupportKHR vkGetPhysicalDeviceSurfaceSupportKHRFn;
        static PFN_vkEnumerateDeviceExtensionProperties vkEnumerateDeviceExtensionPropertiesFn;
        static PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR vkGetPhysicalDeviceSurfaceCapabilitiesKHRFn;
        static PFN_vkGetPhysicalDeviceSurfaceFormatsKHR vkGetPhysicalDeviceSurfaceFormatsKHRFn;
        static PFN_vkGetPhysicalDeviceSurfacePresentModesKHR vkGetPhysicalDeviceSurfacePresentModesKHRFn;
        static PFN_vkCreateDevice vkCreateDeviceFn;
        static PFN_vkGetDeviceQueue vkGetDeviceQueueFn;
        static PFN_vkCreateSwapchainKHR vkCreateSwapchainKHRFn;
        static PFN_vkGetSwapchainImagesKHR vkGetSwapchainImagesKHRFn;
        static PFN_vkDestroySwapchainKHR vkDestroySwapchainKHRFn;
        static PFN_vkCreateImageView vkCreateImageViewFn;
        static PFN_vkDestroyImageView vkDestroyImageViewFn;
        static PFN_vkCreateRenderPass vkCreateRenderPassFn;
        static PFN_vkCreateFramebuffer vkCreateFramebufferFn;
        static PFN_vkDestroyFramebuffer vkDestroyFramebufferFn;
        static PFN_vkCreateImage vkCreateImageFn;
        static PFN_vkGetImageMemoryRequirements vkGetImageMemoryRequirementsFn;
        static PFN_vkBindImageMemory vkBindImageMemoryFn;
        static PFN_vkDestroyImage vkDestroyImageFn;
        static PFN_vkCreateCommandPool vkCreateCommandPoolFn;
        static PFN_vkCreateSemaphore vkCreateSemaphoreFn;
        static PFN_vkCreateFence vkCreateFenceFn;
        static PFN_vkCreatePipelineLayout vkCreatePipelineLayoutFn;
        static PFN_vkCreateGraphicsPipelines vkCreateGraphicsPipelinesFn;
        static PFN_vkDestroyShaderModule vkDestroyShaderModuleFn;
        static PFN_vkDestroyDescriptorPool vkDestroyDescriptorPoolFn;
        static PFN_vkDestroyDescriptorSetLayout vkDestroyDescriptorSetLayoutFn;
        static PFN_vkDestroyPipeline vkDestroyPipelineFn;
        static PFN_vkDestroyPipelineLayout vkDestroyPipelineLayoutFn;
        static PFN_vkCmdBindPipeline vkCmdBindPipelineFn;
        static PFN_vkCmdPushConstants vkCmdPushConstantsFn;
        static PFN_vkCmdBindDescriptorSets vkCmdBindDescriptorSetsFn;
        static PFN_vkUpdateDescriptorSets vkUpdateDescriptorSetsFn;
        static PFN_vkAllocateDescriptorSets vkAllocateDescriptorSetsFn;
        static PFN_vkCreateDescriptorSetLayout vkCreateDescriptorSetLayoutFn;
        static PFN_vkCreateDescriptorPool vkCreateDescriptorPoolFn;
        static PFN_vkCreateShaderModule vkCreateShaderModuleFn;
        static PFN_vkCreateQueryPool vkCreateQueryPoolFn;
        static PFN_vkDestroyQueryPool vkDestroyQueryPoolFn;
        static PFN_vkCmdResetQueryPool vkCmdResetQueryPoolFn;
        static PFN_vkCmdWriteTimestamp2 vkCmdWriteTimestamp2Fn;
        static PFN_vkGetQueryPoolResults vkGetQueryPoolResultsFn;
        static PFN_vkCreateSampler vkCreateSamplerFn;
        static PFN_vkDestroySampler vkDestroySamplerFn;
        static PFN_vkCmdPipelineBarrier vkCmdPipelineBarrierFn;
        static PFN_vkCmdCopyBufferToImage vkCmdCopyBufferToImageFn;

        static bool s_IsLoaded;
    };
}