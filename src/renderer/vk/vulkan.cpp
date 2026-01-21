#include "vulkan.hpp"


namespace Sierra::vlk {
    Vulkan::Vulkan() {

    }

    Vulkan::Vulkan(Renderer::Configuration conf): instance(), device(&instance), context(&instance, &device), taskManager(conf.taskManager),
     swapchain(context, *conf.window), loader(context), resManager(conf.resManager), memManager(context), renderManager(context, *taskManager, memManager, {&swapchain, conf.shaderPaths}) { //ah yes very readable

        runThread = std::thread(&Vulkan::run, this);
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

    void Vulkan::run() {
        while(true) {
            RenderStateManager& renderStateManager = resManager->getRenderManager();
            std::optional<RenderStateManager::RenderPacket*> packet = renderStateManager.getFullPacket(); 

            if(!packet.has_value()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1)); // to avoid overworking the cpu for no reason
                continue;
            }

            renderStateManager.releaseOwnership(packet.value());
        }
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