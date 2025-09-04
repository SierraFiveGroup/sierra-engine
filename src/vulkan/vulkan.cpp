#include "vulkan.hpp"

namespace Sierra::vk {
    Vulkan::Vulkan(Window& window): instance(), device(&instance) {
        ctx.instance = &instance;
        ctx.device = &device;
        swapchain = Swapchain(ctx, window);
    }
}