#pragma once

#include <unordered_map>

#include <vulkan/vulkan.h>

#include "core/context.hpp"
#include "util.hpp"
#include "util/hash.hpp"

namespace Sierra::vlk {
    class DescriptorLayout {
        
        public:
            typedef std::unordered_map<VkDevice, std::unordered_map<uint64_t, VkDescriptorSetLayout>> Layouts_t;

            DescriptorLayout() = delete;

            static void destroy(Context& context);

            static VkDescriptorSetLayout getLayout(Context& context, std::vector<VkDescriptorSetLayoutBinding>& bindings);

        private:
            static VkDescriptorSetLayout createLayout(Context& context, std::vector<VkDescriptorSetLayoutBinding>& bindings);

            static Layouts_t layouts;
    };
}