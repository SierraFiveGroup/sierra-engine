#include "vulkan.hpp"


namespace Sierra::vlk {
    Vulkan::Vulkan() {

    }

    Vulkan::Vulkan(Window& window): instance(), device(&instance), context(&instance, &device),
     swapchain(context, window), memLoader(context), memManager(context) {
        Mem::init(context);
    }

    Scene Vulkan::createScene() {
        Scene::Info info = {
            .swapchain = swapchain
        };
        return Scene(context, info);
    }

    std::vector<Res::ResourceAny> Vulkan::loadResources(ResourceManager::LoadPacket loadInfo) {
        for(ResourceManager::LoadInfo& info : loadInfo.loadInfos) {
            if(info.type == ResourceManager::ResourceType::MODEL) {
                Res::Model modelRes{};
                modelRes.isLoaded = VlkModel::isLoaded;
                //modelRes.extraDat = std::make_shared<uint8_t*>(new VlkModel(context, ));
            }
        }

        return {};
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