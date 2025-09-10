#pragma once

#include "vk/rendering/render_pass/render_pass.hpp"
#include "scene_renderer.hpp"

namespace Sierra::vlk {
    class Scene {
        public:
            Scene(Context& context, Swapchain& swapchain);
        private:
            SceneRenderer sceneRenderer;
    };
}