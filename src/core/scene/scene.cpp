#include "scene.hpp"

namespace Sierra {

    Scene::Scene() {

    }

    Scene::Scene(SceneInfo info) {
        initComponents(info);
    }

    void Scene::initComponents(SceneInfo& info) {
        objects = Hive<uint8_t>(info.objCountPerBlock, info.compLoader->getBlockSize());
    }
}