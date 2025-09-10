#include "scene.hpp"

namespace Sierra::vlk {
    Scene::Scene(Context& context, Swapchain& swapchain) {
        sceneRenderer = SceneRenderer(context, swapchain);
    }
}