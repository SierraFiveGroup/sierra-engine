#include "scene.hpp"

namespace Sierra::vlk {
    Scene::Scene(Context& context, Info& info) {
        sceneRenderer = SceneRenderer(context, info.swapchain);
    }
}