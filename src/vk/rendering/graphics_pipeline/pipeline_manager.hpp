#pragma once

#include <vulkan/vulkan.h>

#include "vk/rendering/render_pass/render_pass.hpp"

namespace Sierra::vlk {
    class PipelineManager {
        public:
            struct PipelineInfo {
                RenderPass& renderPass;
                uint32_t subpass;

                uint32_t basePipelineIndex;  
                VkPipeline basePipeline;
            };

            PipelineManager();
        private:


    };
}