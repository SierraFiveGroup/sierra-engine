#include "vulkan.hpp"


namespace Sierra::vlk {
    Vulkan::Vulkan() {

    }

    Vulkan::Vulkan(Window& window): instance(), device(&instance), context(&instance, &device),
     swapchain(context, window), loader(context) {
        Mem::init(context);
    }

    Scene Vulkan::createScene() {
        Scene::Info info = {
            .swapchain = swapchain
        };
        return Scene(context, info);
    }

    std::vector<Res::ResourceAny> Vulkan::loadResources(ResourceManager::_RendererLoadPacket packet) {
        return loader.load(packet);
    }

    Context& Vulkan::getContext() {
        return context;
    }

    Swapchain& Vulkan::getSwapchain() {
        return swapchain;
    }

    void Vulkan::cleanup() {
        Mem::destroy(context);
    }

    Vulkan::~Vulkan() {

    }
}