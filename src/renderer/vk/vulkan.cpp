#include "vulkan.hpp"


namespace Sierra::vlk {
    Vulkan::Vulkan(Window& window): instance(), device(&instance) {
        context.instance = &instance;
        context.device = &device;
        swapchain = Swapchain(context, window);
        Mem::init(context);
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

    Swapchain& Vulkan::getSwapchain() {
        return swapchain;
    }

    Vulkan::~Vulkan() {
        Mem::destroy(context);
    }

    extern "C" int init(Renderer::Configuration configuration, ResourceManager& resourceManager) {
        return 0;
    }

    extern "C" int update() {
        return 0;
    }

    extern "C" int cleanup() {
        return 0;
    }

    extern "C" int setConfiguration(Renderer::Configuration configuration) {
        return 0;
    }

    extern "C" Renderer::Configuration getConfiguration() {
        return {};
    }

    extern "C" Renderer::Error getError(/*int?*/) {
        return {};
    }

    extern "C" void* loadResource(ResourceManager::LoadInfo loadInfo) {
        return nullptr;
    }
}