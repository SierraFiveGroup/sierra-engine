#include "vulkan.hpp"

namespace Sierra::vk {
    Vulkan::Vulkan(): instance() {
        ctx.instance = &instance;
    }
}