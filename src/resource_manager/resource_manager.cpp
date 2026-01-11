#include "resource_manager.hpp"
namespace Sierra {

    ResourceManager::ResourceManager() {

    }

    ResourceManager::ResourceManager(TaskManager& taskManager, LoadFunc loadFunc): taskManager(&taskManager), loadFunc(loadFunc) {

    }

    std::vector<ResourceManager::ResID> ResourceManager::loadResources(LoadPacket& packet) {
        std::vector<ResourceManager::ResID> ids;
        ids.reserve(packet.loadInfos.size());

        std::vector<Res::ResourceAny> resources = loadFunc(std::move(packet)); 
        for(Res::ResourceAny& res : resources) {
            ids.push_back(res.base.id);

            resIDs[res.base.id] = std::move(res); 
        }

        return ids;
    }

    bool ResourceManager::isLoaded(ResID res) {
        Res::ResourceAny& resAny = resIDs[res];
        return resIDs[res].base.isLoaded(resAny);
    }
}