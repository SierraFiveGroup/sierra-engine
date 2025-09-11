#include "vulkan.hpp"

namespace Sierra::vlk {
    Vulkan::Vulkan(Window& window): instance(), device(&instance) {
        ctx.instance = &instance;
        ctx.device = &device;
        swapchain = Swapchain(ctx, window);
    }

    Scene Vulkan::createScene() {
        Scene::Info info = {
            .swapchain = swapchain
        };
        return Scene(ctx, info);
    }
}