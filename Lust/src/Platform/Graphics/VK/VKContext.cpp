#include "VKContext.hpp"
#include "Application.hpp"
#include "Console.hpp"
#include <algorithm>
#include <cassert>
#include <set>
#include <stdexcept>
#include <utility>
#include <array>

#ifdef LUST_DEBUG_MODE
const std::vector<const char*> Lust::VKContext::s_ValidationLayers =
{
    "VK_LAYER_KHRONOS_validation"
};
#endif

bool Lust::VKContext::CheckLayerSupport(uint32_t vkVersion, const std::vector<const char*>& layerList)
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : layerList) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if ((strcmp(layerName, layerProperties.layerName) == 0) && layerProperties.specVersion == vkVersion) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}



const std::vector<const char*> Lust::VKContext::deviceExtensions =
{
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

Lust::VKContext::VKContext(const Window* windowHandle, uint32_t framesInFlight) :
    m_FramesInFlight(framesInFlight), m_IsVSyncEnabled(true)
{
    //#c0392b
    SetClearColor(0xc0 / 255.0f, 0x39 / 255.0f, 0x2b / 255.0f, 1.0f);

    m_IsWindowClosing = windowHandle->TrackWindowClosing();

    CreateInstance();
#ifdef LUST_DEBUG_MODE
    SetupDebugMessage();
#endif
    CreateSurface(windowHandle);
    SelectAdapter();
    BufferizeUniformAttachment();
    GetGPUName();
    CreateDevice();
    CreateViewportAndScissor(windowHandle->GetWidth(), windowHandle->GetHeight());
    CreateSwapChain();
    CreateImageView();
    CreateRenderPass();
    CreateDepthStencilView();
    CreateFramebuffers();
    CreateCommandPool();
    CreateCommandBuffers();
    CreateSyncObjects();
}

Lust::VKContext::~VKContext()
{
    vkDeviceWaitIdle(m_Device);
    for (size_t i = 0; i < m_FramesInFlight; i++)
        vkDestroyFence(m_Device, m_InFlightFences[i], nullptr);
    delete[] m_InFlightFences;

    for (size_t i = 0; i < m_FramesInFlight; i++)
        vkDestroySemaphore(m_Device, m_ImageAvailableSemaphores[i], nullptr);
    delete[] m_ImageAvailableSemaphores;

    for (size_t i = 0; i < m_FramesInFlight; i++)
        vkDestroySemaphore(m_Device, m_RenderFinishedSemaphores[i], nullptr);
    delete[] m_RenderFinishedSemaphores;

    delete[] m_CommandBuffers;

    vkDestroyCommandPool(m_Device, m_CommandPool, nullptr);
    CleanupFramebuffers();
    CleanupDepthStencilView();
    vkDestroyRenderPass(m_Device, m_RenderPass, nullptr);
    CleanupImageView();
    CleanupSwapChain();
    vkDestroyDevice(m_Device, nullptr);
    vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
#ifdef LUST_DEBUG_MODE
    DestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);
#endif

    vkDestroyInstance(m_Instance, nullptr);
}

void Lust::VKContext::SetClearColor(float r, float g, float b, float a)
{
    m_ClearColor.float32[0] = r;
    m_ClearColor.float32[1] = g;
    m_ClearColor.float32[2] = b;
    m_ClearColor.float32[3] = a;
}

uint32_t Lust::VKContext::GetUniformAttachment() const
{
    return m_UniformAttachment;
}

uint32_t Lust::VKContext::GetSmallBufferAttachment() const
{
    return 4;
}

uint32_t Lust::VKContext::GetFramesInFlight() const
{
    return m_FramesInFlight;
}

void Lust::VKContext::FillRenderPass()
{
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_RenderPass;
    renderPassInfo.framebuffer = m_SwapChainFramebuffers[m_CurrentImageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = m_SwapChainExtent;

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = m_ClearColor;
    clearValues[1].depthStencil = { 1.0f, 0 };

    renderPassInfo.clearValueCount = clearValues.size();
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(m_CommandBuffers[m_CurrentBufferIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void Lust::VKContext::SubmitRenderPass()
{
    vkCmdEndRenderPass(m_CommandBuffers[m_CurrentBufferIndex]);
}

void Lust::VKContext::ReceiveCommands()
{
    vkWaitForFences(m_Device, 1, &m_InFlightFences[m_CurrentBufferIndex], VK_TRUE, UINT64_MAX);

    VkResult result = vkAcquireNextImageKHR(m_Device, m_SwapChain, UINT64_MAX, m_ImageAvailableSemaphores[m_CurrentBufferIndex], VK_NULL_HANDLE, &m_CurrentImageIndex);

    if ((result == VK_ERROR_OUT_OF_DATE_KHR) && !(*m_IsWindowClosing)) {
        vkDeviceWaitIdle(m_Device);
        RecreateSwapChain();
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    vkResetFences(m_Device, 1, &m_InFlightFences[m_CurrentBufferIndex]);
    vkResetCommandBuffer(m_CommandBuffers[m_CurrentBufferIndex], 0);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(m_CommandBuffers[m_CurrentBufferIndex], &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

   
}

void Lust::VKContext::DispatchCommands()
{
    if (vkEndCommandBuffer(m_CommandBuffers[m_CurrentBufferIndex]) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { m_ImageAvailableSemaphores[m_CurrentBufferIndex] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_CommandBuffers[m_CurrentBufferIndex];

    VkSemaphore signalSemaphores[] = { m_RenderFinishedSemaphores[m_CurrentBufferIndex] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, m_InFlightFences[m_CurrentBufferIndex]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }
}

void Lust::VKContext::Present()
{
    VkSemaphore signalSemaphores[] = { m_RenderFinishedSemaphores[m_CurrentBufferIndex] };

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { m_SwapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &m_CurrentImageIndex;

    VkResult result = vkQueuePresentKHR(m_PresentQueue, &presentInfo);

    if ((result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) && !(*m_IsWindowClosing)) {
        vkDeviceWaitIdle(m_Device);
        RecreateSwapChain();
    }
    else if ((result != VK_SUCCESS) && !(*m_IsWindowClosing)) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    m_CurrentBufferIndex = (m_CurrentBufferIndex + 1) % m_FramesInFlight;
}

void Lust::VKContext::StageViewportAndScissors()
{
    vkCmdSetViewport(m_CommandBuffers[m_CurrentBufferIndex], 0, 1, &m_Viewport);
    vkCmdSetScissor(m_CommandBuffers[m_CurrentBufferIndex], 0, 1, &m_ScissorRect);
}

void Lust::VKContext::SetVSync(bool enableVSync)
{
    if (m_IsVSyncEnabled != enableVSync)
    {
        m_IsVSyncEnabled = enableVSync;
        RecreateSwapChain();
    }
}

bool Lust::VKContext::IsVSyncEnabled() const
{
    return false;
}

void Lust::VKContext::Draw(uint32_t elements)
{
    vkCmdDrawIndexed(m_CommandBuffers[m_CurrentBufferIndex], elements, 1, 0, 0, 0);
}

const std::string Lust::VKContext::GetGPUName()
{
    VkPhysicalDeviceProperties adapterProperties;
    vkGetPhysicalDeviceProperties(m_Adapter, &adapterProperties);
    m_GPUName = adapterProperties.deviceName;
    return m_GPUName;
}

void Lust::VKContext::WindowResize(uint32_t width, uint32_t height)
{
    vkDeviceWaitIdle(m_Device);
    CreateViewportAndScissor(width, height);

    RecreateSwapChain();
}

VkInstance Lust::VKContext::GetInstance() const
{
    return m_Instance;
}

VkCommandPool Lust::VKContext::GetCommandPool() const
{
    return m_CommandPool;
}

VkQueue Lust::VKContext::GetGraphicsQueue() const
{
    return m_GraphicsQueue;
}

VkPhysicalDevice Lust::VKContext::GetAdapter() const
{
    return m_Adapter;
}

VkDevice Lust::VKContext::GetDevice() const
{
    return m_Device;
}

VkRenderPass Lust::VKContext::GetRenderPass() const
{
    return m_RenderPass;
}

VkCommandBuffer Lust::VKContext::GetCurrentCommandBuffer() const
{
    return m_CommandBuffers[m_CurrentBufferIndex];
}

VkSurfaceKHR Lust::VKContext::GetSurface() const
{
    return m_Surface;
}

uint32_t Lust::VKContext::GetGraphicsQueueFamilyIndex() const
{
    return m_QueueFamilyIndices.graphicsFamily.value();
}

uint32_t Lust::VKContext::GetSwapchainImageCount() const
{
    return m_SwapChainImageCount;
}

void Lust::VKContext::CreateInstance()
{
    uint32_t vkVersion = VK_MAKE_API_VERSION(0, 1, 4, 304);
    VkResult vkr;
#ifdef LUST_DEBUG_MODE
    assert(CheckLayerSupport(vkVersion, s_ValidationLayers));
#endif

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = vkVersion;
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = vkVersion;
    appInfo.apiVersion = vkVersion;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    m_InstanceExtensions.push_back("VK_KHR_surface");

#ifdef LUST_USES_WINDOWS
    m_InstanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif

#ifdef LUST_DEBUG_MODE
    m_InstanceExtensions.push_back("VK_EXT_debug_utils");
#endif

    m_InstanceExtensions.push_back("VK_EXT_layer_settings");

    createInfo.enabledExtensionCount = static_cast<uint32_t>(m_InstanceExtensions.size());
    createInfo.ppEnabledExtensionNames = m_InstanceExtensions.data();

    std::vector<const char*> layers;
    layers.push_back("VK_LAYER_KHRONOS_synchronization2");

    assert(CheckLayerSupport(vkVersion, layers));

#ifdef RENDER_DEBUG_MODE

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

    layers.push_back("VK_LAYER_KHRONOS_validation");

    PopulateDebugMessengerCreateInfo(debugCreateInfo);
    createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
#else

    createInfo.pNext = nullptr;

#endif

    createInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
    createInfo.ppEnabledLayerNames = layers.data();

    vkr = vkCreateInstance(&createInfo, nullptr, &m_Instance);
    assert(vkr == VK_SUCCESS);
}

void Lust::VKContext::CreateSurface(const Window* windowHandle)
{
    VkResult vkr;

#ifdef LUST_USES_WINDOWS
    VkWin32SurfaceCreateInfoKHR creationInfo = {};
    creationInfo.hinstance = std::any_cast<HINSTANCE>(windowHandle->GetInstance());
    creationInfo.hwnd = std::any_cast<HWND>(windowHandle->GetNativePointer());
    creationInfo.pNext = nullptr;
    creationInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    creationInfo.flags = 0;

    vkr = vkCreateWin32SurfaceKHR(m_Instance, &creationInfo, nullptr, &m_Surface);

#endif // RENDER_USES_WINDOWS

    assert(vkr == VK_SUCCESS);
}

void Lust::VKContext::SelectAdapter()
{
    VkResult vkr;
    uint32_t deviceCount = 0;
    vkr = vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);
    assert((vkr == VK_SUCCESS) && (deviceCount > 0));

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkr = vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());
    assert(vkr == VK_SUCCESS);

#if 0

    std::vector<VkPhysicalDeviceProperties> GPUProperties;

    for (const auto& device : devices)
    {
        VkPhysicalDeviceProperties adapterProperties;
        vkGetPhysicalDeviceProperties(device, &adapterProperties);
        GPUProperties.push_back(adapterProperties);
    }

#endif

    for (const auto& device : devices) {
        if (IsDeviceSuitable(device)) {
            m_Adapter = device;
            break;
        }
    }

    assert(m_Adapter != VK_NULL_HANDLE);
}

bool Lust::VKContext::IsDeviceSuitable(VkPhysicalDevice adapter)
{
    QueueFamilyIndices indices = FindQueueFamilies(adapter);

    bool extensionsSupported = CheckDeviceExtensionSupport(adapter);

    bool swapChainAdequate = false;
    if (extensionsSupported)
    {
        SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(adapter);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    return indices.isComplete() && extensionsSupported && swapChainAdequate;
}

Lust::QueueFamilyIndices Lust::VKContext::FindQueueFamilies(VkPhysicalDevice adapter)
{
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(adapter, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(adapter, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies)
    {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            indices.graphicsFamily = i;
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(adapter, i, m_Surface, &presentSupport);
        if (presentSupport)
            indices.presentFamily = i;

        if (indices.isComplete())
            break;
        i++;
    }

    return indices;
}

bool Lust::VKContext::CheckDeviceExtensionSupport(VkPhysicalDevice adapter)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(adapter, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(adapter, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

Lust::SwapChainSupportDetails Lust::VKContext::QuerySwapChainSupport(VkPhysicalDevice adapter)
{
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(adapter, m_Surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(adapter, m_Surface, &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(adapter, m_Surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(adapter, m_Surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(adapter, m_Surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

void Lust::VKContext::BufferizeUniformAttachment()
{
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(m_Adapter, &deviceProperties);
    m_UniformAttachment = (uint32_t)deviceProperties.limits.minUniformBufferOffsetAlignment;
}

void Lust::VKContext::CreateDevice()
{
    VkResult vkr;
    m_QueueFamilyIndices = FindQueueFamilies(m_Adapter);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = { m_QueueFamilyIndices.graphicsFamily.value(), m_QueueFamilyIndices.presentFamily.value() };

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

#ifdef LUST_DEBUG_MODE
    createInfo.enabledLayerCount = static_cast<uint32_t>(s_ValidationLayers.size());
    createInfo.ppEnabledLayerNames = s_ValidationLayers.data();
#else
    createInfo.enabledLayerCount = 0;
#endif
    vkr = vkCreateDevice(m_Adapter, &createInfo, nullptr, &m_Device);
    assert(vkr == VK_SUCCESS);

    vkGetDeviceQueue(m_Device, m_QueueFamilyIndices.graphicsFamily.value(), 0, &m_GraphicsQueue);
    vkGetDeviceQueue(m_Device, m_QueueFamilyIndices.presentFamily.value(), 0, &m_PresentQueue);
}

void Lust::VKContext::CreateViewportAndScissor(uint32_t width, uint32_t height)
{
    m_Viewport.x = 0.0f;
    m_Viewport.y = 0.0f;
    m_Viewport.width = (float)width;
    m_Viewport.height = (float)height;
    m_Viewport.minDepth = 0.0f;
    m_Viewport.maxDepth = 1.0f;

    m_ScissorRect.offset.x = 0;
    m_ScissorRect.offset.y = 0;
    m_ScissorRect.extent.height = height;
    m_ScissorRect.extent.width = width;
}

void Lust::VKContext::CreateSwapChain()
{
    VkResult vkr;
    SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(m_Adapter);

    VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = m_Surface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = FindQueueFamilies(m_Adapter);
    uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    vkr = vkCreateSwapchainKHR(m_Device, &createInfo, nullptr, &m_SwapChain);
    assert(vkr == VK_SUCCESS);

    vkGetSwapchainImagesKHR(m_Device, m_SwapChain, &imageCount, nullptr);
    m_SwapChainImageCount = imageCount;
    m_SwapChainImages = new VkImage[m_SwapChainImageCount];
    vkGetSwapchainImagesKHR(m_Device, m_SwapChain, &m_SwapChainImageCount, m_SwapChainImages);

    m_SwapChainImageFormat = surfaceFormat.format;
    m_SwapChainExtent = extent;
}

VkSurfaceFormatKHR Lust::VKContext::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR Lust::VKContext::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    for (const auto& availablePresentMode : availablePresentModes) {
        if ((availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) && m_IsVSyncEnabled) {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_IMMEDIATE_KHR;
}

VkExtent2D Lust::VKContext::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }
    else {
        VkExtent2D actualExtent = {
            (uint32_t)m_Viewport.width,
            (uint32_t)m_Viewport.height
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

void Lust::VKContext::CleanupSwapChain()
{
    delete[] m_SwapChainImages;
    vkDestroySwapchainKHR(m_Device, m_SwapChain, nullptr);
}

void Lust::VKContext::RecreateSwapChain()
{
    CleanupFramebuffers();
    CleanupDepthStencilView();
    CleanupImageView();
    CleanupSwapChain();

    CreateSwapChain();
    CreateImageView();
    CreateDepthStencilView();
    CreateFramebuffers();
}

void Lust::VKContext::CreateImageView()
{
    VkResult vkr;
    m_SwapChainImageViews = new VkImageView[m_SwapChainImageCount];

    for (size_t i = 0; i < m_SwapChainImageCount; i++) {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = m_SwapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = m_SwapChainImageFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        vkr = vkCreateImageView(m_Device, &createInfo, nullptr, &m_SwapChainImageViews[i]);
        assert(vkr == VK_SUCCESS);
    }
}

void Lust::VKContext::CleanupImageView()
{
    for (size_t i = 0; i < m_SwapChainImageCount; i++)
    {
        vkDestroyImageView(m_Device, m_SwapChainImageViews[i], nullptr);
    }
    delete[] m_SwapChainImageViews;
}

void Lust::VKContext::CreateRenderPass()
{
    VkResult vkr;
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = m_SwapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = VK_FORMAT_D24_UNORM_S8_UINT;
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = attachments.size();
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    vkr = vkCreateRenderPass(m_Device, &renderPassInfo, nullptr, &m_RenderPass);
    assert(vkr == VK_SUCCESS);
}

void Lust::VKContext::CreateFramebuffers()
{
    VkResult vkr;
    m_SwapChainFramebuffers = new VkFramebuffer[m_SwapChainImageCount];

    for (size_t i = 0; i < m_SwapChainImageCount; i++) {
        VkImageView attachments[] = {
            m_SwapChainImageViews[i],
            m_DepthStencilView
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = m_RenderPass;
        framebufferInfo.attachmentCount = 2;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = (uint32_t)m_Viewport.width;
        framebufferInfo.height = (uint32_t)m_Viewport.height;
        framebufferInfo.layers = 1;

        vkr = vkCreateFramebuffer(m_Device, &framebufferInfo, nullptr, &m_SwapChainFramebuffers[i]);
        assert(vkr == VK_SUCCESS);
    }
}

void Lust::VKContext::CleanupFramebuffers()
{
    for (size_t i = 0; i < m_SwapChainImageCount; i++)
    {
        vkDestroyFramebuffer(m_Device, m_SwapChainFramebuffers[i], nullptr);
    }
    delete[] m_SwapChainFramebuffers;
}

void Lust::VKContext::CreateDepthStencilView()
{
    VkResult vkr;

    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = m_SwapChainExtent.width;
    imageInfo.extent.height = m_SwapChainExtent.height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = VK_FORMAT_D24_UNORM_S8_UINT;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    vkr = vkCreateImage(m_Device, &imageInfo, nullptr, &m_DepthStencilBuffer);
    (vkr == VK_SUCCESS);

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(m_Device, m_DepthStencilBuffer, &memRequirements);

    VkPhysicalDeviceMemoryProperties memProperties;
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = 0xffffffffu;

    vkGetPhysicalDeviceMemoryProperties(m_Adapter, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((memRequirements.memoryTypeBits & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) == VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
            allocInfo.memoryTypeIndex = i;
        }
    }


    vkr = vkAllocateMemory(m_Device, &allocInfo, nullptr, &m_DepthStencilMemory);
    assert(vkr == VK_SUCCESS);

    vkBindImageMemory(m_Device, m_DepthStencilBuffer, m_DepthStencilMemory, 0);

    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = m_DepthStencilBuffer;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = VK_FORMAT_D24_UNORM_S8_UINT;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    vkr = vkCreateImageView(m_Device, &viewInfo, nullptr, &m_DepthStencilView);
    assert(vkr == VK_SUCCESS);
}

void Lust::VKContext::CleanupDepthStencilView()
{
    vkDestroyImageView(m_Device, m_DepthStencilView, nullptr);
    vkDestroyImage(m_Device, m_DepthStencilBuffer, nullptr);
    vkFreeMemory(m_Device, m_DepthStencilMemory, nullptr);
}

void Lust::VKContext::CreateCommandPool()
{
    VkResult vkr;
    QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(m_Adapter);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    vkr = vkCreateCommandPool(m_Device, &poolInfo, nullptr, &m_CommandPool);
    assert(vkr == VK_SUCCESS);
}

void Lust::VKContext::CreateCommandBuffers()
{
    VkResult vkr;
    m_CommandBuffers = new VkCommandBuffer[m_FramesInFlight];

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_CommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = m_FramesInFlight;

    vkr = vkAllocateCommandBuffers(m_Device, &allocInfo, m_CommandBuffers);
    assert(vkr == VK_SUCCESS);
}

void Lust::VKContext::CreateSyncObjects()
{
    VkResult vkr;
    m_ImageAvailableSemaphores = new VkSemaphore[m_FramesInFlight];
    m_RenderFinishedSemaphores = new VkSemaphore[m_FramesInFlight];
    m_InFlightFences = new VkFence[m_FramesInFlight];

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < m_FramesInFlight; i++) {
        vkr = vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[i]);
        assert(vkr == VK_SUCCESS);
        vkr = vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[i]);
        assert(vkr == VK_SUCCESS);
        vkr = vkCreateFence(m_Device, &fenceInfo, nullptr, &m_InFlightFences[i]);
        assert(vkr == VK_SUCCESS);
    }
}

#ifdef LUST_DEBUG_MODE

void Lust::VKContext::SetupDebugMessage()
{
    VkResult VKR;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    PopulateDebugMessengerCreateInfo(createInfo);

    VKR = CreateDebugUtilsMessengerEXT(m_Instance, &createInfo, nullptr, &m_DebugMessenger);
    assert(VKR == VK_SUCCESS);
}

void Lust::VKContext::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = []
    (
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageTypes,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData
        ) -> VkBool32
        {
            Console::CoreLog("validation layer: {}", pCallbackData->pMessage);
            return VK_FALSE;
        };
}

VkResult Lust::VKContext::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void Lust::VKContext::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

#endif