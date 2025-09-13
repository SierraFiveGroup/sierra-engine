#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "vk/rendering/render_pass/render_pass.hpp"

namespace Sierra::vlk {
    class PipelineManager {
        public:
            struct PipelineInfo { // do we need tesselation??
                //other than that we only really need to pass vertexInput
                //everything else we can make assumptions about
                RenderPass& renderPass;
                uint32_t subpass;

                uint32_t basePipelineIndex;  
                VkPipeline basePipeline;

                VkPipelineVertexInputStateCreateInfo inputState;

                std::vector<VkPipelineShaderStageCreateInfo> stages;
            };

            PipelineManager();
            PipelineManager(Context& context, PipelineInfo& info);
        private:


    };
}