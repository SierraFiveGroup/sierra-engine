#include "engine.hpp"

namespace Sierra {
    Engine::Engine(std::string &windowTitle, Resolution dimensions, uint32_t mempoolSize): mWindow(windowTitle, dimensions, API::vulkan), mMempool(mempoolSize),
     mALDevice() {
        if (!mALDevice.isValid()) {
            ERROR("OpenAL Device Error");
        }
    }

    void Engine::start() {
        if (mActiveScene != nullptr) { // FIXME tf does this even evaluate to??
            mActiveScene->update();
        }
    }
        
    void Engine::update() {
        mWindow.update();
        
        if (mActiveScene != nullptr) {
            mActiveScene->update();
        }
    }

    bool Engine::loadScene(Scene *scene) {
        if (scene != nullptr) {
            mActiveScene = scene; //This should kill the objects in the previous scene (?) since they're smart pointers
            return true;
        }

        return false;
    }

    void Engine::unloadScene() {
        mActiveScene.reset();
    }        
}