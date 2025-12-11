/*
#include "EngineAPI.h"
#include "scene/scene.hpp"
#include <string>

#include "io/logging/logger.hpp"
#include "util/utilities.hpp"
namespace Sierra {
    class SceneWrapper : public Scripting::Scene {
        Sierra::Scene mScene;

        public:
            std::shared_ptr<Camera> GetCamera() override {
                return mScene.getCamera().getLock();
            }

            template <typename... Args>
            Sierra::Scripting::GameObject CreateObject(Args&... args) {
                return mScene.createObject<Object>(args...);
            }

            void DestroyObject(Sierra::Scripting::GameObject &object) {
                //mScene.destroyObject(ResourceRef<Object>(std::weak_ptr(object)));
                WARN("Not implemented");
            }
    };
}

API_EXPORT Sierra::Scripting::Scene *CreateScene() {
    return new Sierra::SceneWrapper();
}*/