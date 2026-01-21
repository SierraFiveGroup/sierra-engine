#include "resource_manager.hpp"
namespace Sierra {

    //MEGA TODO cleanup unused resBuffers
    ResourceManager::ResourceManager() {

    }
    
    ResourceManager::ResourceManager(Info info): taskManager(info.taskManager), loadFunc(info.loadFunc),
     renderManager(info.framebufferCount) {

    }


    std::vector<Res::ResID> ResourceManager::loadResources(LoadPacket& packet) {
        std::vector<Res::ResID> ids;
        ids.reserve(packet.loadInfos.size());

        _RendererLoadPacket rendererPacket{
            .taskManager = taskManager
        };
        rendererPacket.loadInfos.reserve(packet.loadInfos.size());

        BaseResourceBuffers baseBuffers;
        baseBuffers.models.reserve(packet.loadInfos.size());
        baseBuffers.textures.reserve(packet.loadInfos.size()); // assume worst case so no reallocations take place

        for(LoadInfo& info : packet.loadInfos) {
            switch(info.type) {
                case Res::Type::MODEL:
                    rendererPacket.loadInfos.emplace_back(loadModel(baseBuffers, info));
                    break;
                case Res::Type::TEXTURE:
                    rendererPacket.loadInfos.emplace_back(loadTexture(baseBuffers, info));
                    break;
                default:
                    throw std::runtime_error("Unsupported resource type"); // TODO set error when implementing them
            }
        }

        std::vector<Res::ResourceAny> resources = loadFunc(std::move(rendererPacket));

        for(Res::ResourceAny& res : resources) {
            res.base.id = resIDInc;
            resIDInc++;

            ids.push_back(res.base.id);
            resIDs[res.base.id] = std::move(res); 
        }

        return ids;
    }

    bool ResourceManager::isLoaded(Res::ResID res) {
        Res::ResourceAny& resAny = resIDs[res];
        return resIDs[res].base.isLoaded(resAny);
    }

    ResourceManager::_RendererLoadInfo ResourceManager::loadModel(BaseResourceBuffers& baseBuffers, LoadInfo loadInfo) {
        _RendererLoadInfo rendererInfo;

        baseBuffers.models.emplace_back(*taskManager, loadInfo.path);

        rendererInfo.type = Res::Type::MODEL;
        rendererInfo.baseRes.model = &baseBuffers.models.back();

        return rendererInfo;
    }

    ResourceManager::_RendererLoadInfo ResourceManager::loadTexture(BaseResourceBuffers& baseBuffers, LoadInfo loadInfo) {
        _RendererLoadInfo rendererInfo;

        baseBuffers.textures.emplace_back(*taskManager, loadInfo.path);

        rendererInfo.type = Res::Type::TEXTURE;
        rendererInfo.baseRes.texture = &baseBuffers.textures.back();

        return rendererInfo;
    }

    RenderStateManager& ResourceManager::getRenderManager() {
        return renderManager;
    }

    ResourceManager::ResourceManager(ResourceManager&& other) {
        resIDs = std::move(other.resIDs);
        baseResBuffers = std::move(other.baseResBuffers);
        taskManager = other.taskManager;
        loadFunc = other.loadFunc;
        resIDInc = other.resIDInc;
    }

    void ResourceManager::operator=(ResourceManager&& other) {
        resIDs = std::move(other.resIDs);
        baseResBuffers = std::move(other.baseResBuffers);
        taskManager = other.taskManager;
        loadFunc = other.loadFunc;
        resIDInc = other.resIDInc;
    }

    void ResourceManager::cleanup() {
        resIDs.clear(); // required before de-initing vulkan
    }
}