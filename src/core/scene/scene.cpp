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

    Object Scene::createObject() {
        return Object(objects.insert((uint8_t)0));
    }

    void Scene::eraseObject(Object obj) {
        objects.erase(obj.row);
    }
}
