#pragma once

#include "vk/rendering/render_pass/render_pass.hpp"
#include "vk/rendering/swapchain/swapchain.hpp"
#include "vk/core/context.hpp"

namespace Sierra::vlk {
    class SceneRenderer {
        public:
            SceneRenderer();
            SceneRenderer(Context& context, Swapchain& swapchain);
        private:
            void createRenderPass(Context& context, Swapchain& swapchain);

            RenderPass mainPass;
    };
}