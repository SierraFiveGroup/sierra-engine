#pragma once

#include <vulkan/vulkan.h>
#include "../../util.hpp"

namespace Sierra::vk { 
    class Instance {
        public:
            Instance();

            Instance(Instance&) = delete;
            Instance(Instance&&) = delete;

            ~Instance();

            VkInstance getVkInstance();

        private:
            VkInstance vkInstance;
    };
}