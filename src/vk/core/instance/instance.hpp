#pragma once

#include <vector>
#include <cstring>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "../../util.hpp"
#include "io/logging/logger.hpp"

namespace Sierra::vlk { 
    class Instance {
        public:
            Instance();

            Instance(Instance&) = delete;
            Instance(Instance&&) = delete;

            ~Instance();

            VkInstance getInstance();

        private:
#ifdef DEBUG 
            bool checkLayerSupport();
#endif

            std::vector<const char*> getExtentions();

            VkInstance vkInstance;
    };
}