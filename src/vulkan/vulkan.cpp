#include "vulkan.hpp"

namespace Sierra::vk {
    Vulkan::Vulkan(): instance(), device(&instance) {
        ctx.instance = &instance;
        ctx.device = &device;
    }
}