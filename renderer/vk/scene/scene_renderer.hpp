#pragma once

#include "rendering/render_pass/render_pass.hpp"
#include "rendering/swapchain/swapchain.hpp"
#include "core/context.hpp"

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