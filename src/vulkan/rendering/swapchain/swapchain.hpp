#pragma once

#include <vulkan/vulkan.h>

#include "../../core/context.hpp"

namespace Sierra::vk {
    class Swapchain {
        public:
            Swapchain(Context& context);

            Swapchain(Swapchain&);
            Swapchain(Swapchain&&);

            ~Swapchain();
        private:
            void createSwapchain(Context& context);
            VkFormat selectImageFormat(Context& context);

            VkSwapchainKHR vkSwapchain;
    };
}