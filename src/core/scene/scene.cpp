#include "scene.hpp"

#include "resource_manager/rendering/render_state_manager.hpp"

namespace Sierra {

    Scene::Scene() {

    }

    Scene::Scene(SceneInfo info): resManager(info.resManager) {
        initComponents(info);
    }

    void Scene::initComponents(SceneInfo& info) {
        objects = Hive<uint8_t>(info.objCountPerBlock, info.compLoader->getBlockSize() + sizeof(Object));
    }

    Object* Scene::createObject(ObjectBlueprint& objBlueprint) {
        uint8_t* row = objects.insert(0);
        new ((Object*)row) Object(objBlueprint, row); // TODO long term can we guarantee `row == this` for obj or this will be changed

        return (Object*)row;
    }

    void Scene::update() {
        submitRenderPacket();
    }

    void Scene::submitRenderPacket() {
        RenderStateManager& manager = resManager->getRenderManager();

        std::optional<RenderStateManager::RenderPacket*> packetOpt = manager.getEmptyPacket();
        if(!packetOpt.has_value()) return; // no work, no bitches no nuthin
        RenderStateManager::RenderPacket* packet = packetOpt.value();

        //TODO some kind of caching to associate objects with correct batch more easily
        //also whatever I am doing here is temporary dw

        for(auto it = objects.begin(); it != objects.end(); it++) {
            Object& obj = *((Object*)&*it);

            packet->batches.push_back({
                obj.getShaderID(), obj.getModelID(), {*obj.getComponent<Component::Transform3D>(SIERRA_COMPONENT_TRANSFORM_3D)}
            });
        }

        // TODO: if compiler complains about this, just comment for now
        std::unordered_map<std::pair<Res::ResID, Res::ResID>, std::vector<Component::Transform3D>, pair_hash> batchMap;

        for (auto it = objects.begin(); it != objects.end(); it++) {
            Object& obj = *((Object*)&*it);

            // Push into render batches based on shader and model IDs
            batchMap[{obj.getShaderID(), obj.getModelID()}].push_back(*obj.getComponent<Component::Transform3D>(SIERRA_COMPONENT_TRANSFORM_3D));
        }

        // Push into the actual packet
        for (const auto& [keyPair, transformVectors] : batchMap) {
            packet->batches.push_back({
                keyPair.first,   // shader ID
                keyPair.second,  // model ID
                transformVectors
            });
        }
    }

    void Scene::eraseObject(Object obj) {
        objects.erase(obj.row);
    }
}
