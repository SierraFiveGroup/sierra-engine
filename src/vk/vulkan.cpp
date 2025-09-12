#include "vulkan.hpp"

namespace Sierra::vlk {
    Vulkan::Vulkan(Window& window): instance(), device(&instance) {
        context.instance = &instance;
        context.device = &device;
        swapchain = Swapchain(context, window);
    }

    Scene Vulkan::createScene() {
        Scene::Info info = {
            .swapchain = swapchain
        };
        return Scene(context, info);
    }

    Context& Vulkan::getContext() {
        return context;
    }
}