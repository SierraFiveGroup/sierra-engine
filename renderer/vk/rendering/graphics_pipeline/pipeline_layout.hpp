#pragma once

#include <vulkan/vulkan.h>

#include <vector>
#include <unordered_map>

#include "core/context.hpp"
#include "res/shader/descriptor/descriptor_layout.hpp"
#include "util/hash.hpp"

namespace Sierra::vlk {
    class PipelineLayout {
        public:
            typedef std::unordered_map<VkDevice, std::unordered_map<uint64_t, VkPipelineLayout>> Layouts_t;

            PipelineLayout() = delete;

            static VkPipelineLayout getLayout(Context& context, VkDescriptorSetLayout setLayout, std::vector<VkPushConstantRange>& pushConstantRanges);

            static void destroy(Context& context);
        private:
            static VkPipelineLayout createLayout(Context& context, VkDescriptorSetLayout setLayout, std::vector<VkPushConstantRange>& pushConstantRanges);

            static uint64_t getHash(Context& context, VkDescriptorSetLayout setLayout, std::vector<VkPushConstantRange>& pushConstantRanges);

            static Layouts_t layouts;

    };
}