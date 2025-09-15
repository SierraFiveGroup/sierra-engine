#pragma once

#include <vector>
#include <unordered_map>

#include <vulkan/vulkan.h>

#include "vk/rendering/render_pass/render_pass.hpp"
#include "pipeline_layout.hpp"
#include "util/hash.hpp"

namespace Sierra::vlk {
    class GraphicsPipeline {
        public:
            typedef std::unordered_map<VkDevice, std::unordered_map<uint64_t, VkPipeline>> Pipelines_t;

            struct PipelineInfo { // do we need tesselation??
                //other than that we only really need to pass vertexInput
                //everything else we can make assumptions about
                RenderPass* renderPass;
                uint32_t subpass;

                VkPipelineLayout layout;

                uint32_t basePipelineIndex;  
                VkPipeline basePipeline;

                VkPipelineVertexInputStateCreateInfo inputState;

                std::vector<VkPipelineShaderStageCreateInfo> stages;
            };

            GraphicsPipeline() = delete;

            static VkPipeline getPipeline(Context& context, PipelineInfo& info);

            static void destroy(Context& context);
        private:
            static uint64_t getHash(Context& context, PipelineInfo& info);

            static VkPipeline createPipeline(Context& context, PipelineInfo& info);

            static Pipelines_t pipelines;



    };
}