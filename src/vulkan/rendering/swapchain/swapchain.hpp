#pragma once

#include <vulkan/vulkan.h>

#include "../../core/context.hpp"
#include "window/window.hpp"

namespace Sierra::vk {
    class Swapchain {
        public:
            Swapchain();
            Swapchain(Context& context, Window& window);

            Swapchain(Swapchain&) = delete;

            void operator=(Swapchain&&);
            Swapchain(Swapchain&&);

            ~Swapchain();
        private:
            void createWindowSurface(Window& window);
            void createSwapchain();
            VkSurfaceFormatKHR selectImageFormat();


            VkSwapchainKHR vkSwapchain;
            VkSurfaceKHR surface;

            Context* context;

            const uint32_t PREFERRED_IMAGE_COUNT = 2; 

    };
}