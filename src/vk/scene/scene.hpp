#pragma once

#include <vector>

#include "vk/rendering/render_pass/render_pass.hpp"
#include "vk/res/shader/shader.hpp"
#include "scene_renderer.hpp"

namespace Sierra::vlk {
    class Scene {
        public:
            struct Info {
                std::vector<Shader*> shaders;
                Swapchain& swapchain;
            };

            Scene(Context& context, Info& info);
        private:
            SceneRenderer sceneRenderer;
    };
}