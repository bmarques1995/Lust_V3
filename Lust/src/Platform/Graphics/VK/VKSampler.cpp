#include "VKSampler.hpp"
#include <cassert>
#include "VKFunctions.hpp"

Lust::VKSampler::VKSampler(const VKContext* context, const SamplerInfo& info) :
    m_Context(context)
{
	m_Info = info;
    VkResult vkr;
    auto device = m_Context->GetDevice();
    auto adapter = m_Context->GetAdapter();

    VkPhysicalDeviceProperties properties{};
    VKFunctions::vkGetPhysicalDevicePropertiesFn(adapter, &properties);

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = GetNativeFilter(info.GetFilter());
    samplerInfo.minFilter = GetNativeFilter(info.GetFilter());
    samplerInfo.addressModeU = GetNativeAddressMode(info.GetAddressMode());
    samplerInfo.addressModeV = GetNativeAddressMode(info.GetAddressMode());
    samplerInfo.addressModeW = GetNativeAddressMode(info.GetAddressMode());
    samplerInfo.anisotropyEnable = info.GetFilter() == SamplerFilter::ANISOTROPIC ? VK_TRUE : VK_FALSE;
    samplerInfo.maxAnisotropy = std::min<float>(properties.limits.maxSamplerAnisotropy, (1 << (uint32_t)info.GetAnisotropicFactor()) * 1.0f);
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_TRUE;
    samplerInfo.compareOp = (VkCompareOp)((uint32_t)info.GetComparisonPassMode());
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = FLT_MAX;

    vkr = VKFunctions::vkCreateSamplerFn(device, &samplerInfo, nullptr, &m_Sampler);
    assert(vkr == VK_SUCCESS);
}

Lust::VKSampler::~VKSampler()
{
    auto device = m_Context->GetDevice();
    VKFunctions::vkDeviceWaitIdleFn(device);
    VKFunctions::vkDestroySamplerFn(device, m_Sampler, nullptr);
}

VkSampler Lust::VKSampler::GetSampler() const
{
	return m_Sampler;
}

VkFilter Lust::VKSampler::GetNativeFilter(SamplerFilter filter)
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

VkSamplerAddressMode Lust::VKSampler::GetNativeAddressMode(AddressMode addressMode)
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
