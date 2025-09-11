#pragma once

#include <unordered_map>

#include <vulkan/vulkan.h>

#include "vk/core/context.hpp"
#include "vk/util.hpp"

namespace Sierra::vlk {
    class DescriptorLayout {
        
        public:
            typedef std::unordered_map<VkDevice, std::unordered_map<uint64_t, VkDescriptorSetLayout>> Layouts_t;

            static void destroy(Context& context);

            static VkDescriptorSetLayout getLayout(Context& context);

        private:
            static Layouts_t layouts;
    };
}