#pragma once

#include <vector>
#include <cstring>
#include <stacktrace>

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_NONE
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
            void createInstace();
#ifdef DEBUG 
            void createDebugMessenger();
            void destroyDebugMessenger();
            bool checkLayerSupport();
#endif

            std::vector<const char*> getExtentions();

            VkInstance vkInstance;
            VkDebugUtilsMessengerEXT dbgMessenger;
    };
}