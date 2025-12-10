#include "sampler.hpp"

namespace Sierra::vlk {
    Sampler::Sampler(): context(), sampler() {

    }

    Sampler::Sampler(Context& context, Info info): context(&context), sampler() {
        createSampler(info);
    }
    
    void Sampler::createSampler(Info& info) {
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = info.filter;
        samplerInfo.minFilter = info.filter;
        samplerInfo.mipmapMode = info.mipmapMode;
        samplerInfo.addressModeU = info.addressMode;
        samplerInfo.addressModeV = info.addressMode;
        samplerInfo.addressModeW = info.addressMode;
        samplerInfo.mipLodBias = info.lodBias;
        samplerInfo.anisotropyEnable = info.anisotropy;
        samplerInfo.maxAnisotropy = info.maxAnisotropy;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.minLod = info.minLod;
        samplerInfo.maxLod = info.maxLod;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;

        VK_ERR(vkCreateSampler(context->device->getDevice(), &samplerInfo, nullptr, &sampler));
    }

    VkSampler Sampler::getSampler() {
        return sampler;
    }

    void Sampler::operator=(Sampler&& other) {
        sampler = std::move(other.sampler);
        context = std::move(other.context);

        other.sampler = VK_NULL_HANDLE;
    }

    Sampler::Sampler(Sampler&& other) {
        sampler = std::move(other.sampler);
        context = std::move(other.context);

        other.sampler = VK_NULL_HANDLE;
    }

    Sampler::~Sampler() {
        if(sampler)
            vkDestroySampler(context->device->getDevice(), sampler, nullptr);
    }
}