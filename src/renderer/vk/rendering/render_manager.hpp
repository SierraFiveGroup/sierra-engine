#pragma once

#include "render_pass/render_pass.hpp"
#include "swapchain/swapchain.hpp"
#include "graphics_pipeline/graphics_pipeline.hpp"
#include "renderer/vk/res/shader/loader/shader_loader.hpp"

#include "renderer/renderer.hpp"

namespace Sierra::vlk {
    class RenderManager {
        public:
            struct Info{
                Swapchain* swapchain;

                std::vector<Renderer::ShaderPair> shaders;
            };

            RenderManager();
            RenderManager(Context& context, TaskManager& taskManager, MemoryManager& memManager, Info info);

        private:
            void createMainPass(Info& info, MemoryManager& memManager);
            void createGraphicsPipelines(Info& info);

            Context* context;

            RenderPass mainPass; // maybe add secondary for ui and stuff
            //also post processing!
            std::vector<VkPipeline> pipelines;

            ShaderLoader shaderLoader;
    };
}