#include "fence.hpp"

namespace Sierra::vlk {

    Fence::Fence(): context(), fence() {

    }

    Fence::Fence(Context& context): context(&context), fence() {
        createFence();
    }

    void Fence::createFence() {
        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

        VK_ERR(vkCreateFence(context->device->getDevice(), &fenceInfo, nullptr, &fence));
    }

    VkFence Fence::getFence() {
        return fence;
    }

    Fence::Fence(Fence&& other) {
        fence = other.fence;
        context = other.context;

        other.fence = VK_NULL_HANDLE;
        other.context = nullptr;
    }

    void Fence::operator=(Fence&& other) {
        fence = other.fence;
        context = other.context;

        other.fence = VK_NULL_HANDLE;
        other.context = nullptr;
    }

    Fence::~Fence() {
        if (fence)
            vkDestroyFence(context->device->getDevice(), fence, nullptr);
    }

}