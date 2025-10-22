#pragma once

#include <vulkan/vulkan.h>

#include <algorithm>

#include "../../core/context.hpp"
#include "window/window.hpp"
#include "vk/res/mem/image/image_view.hpp"

namespace Sierra::vlk {
    class Swapchain {
        public:
            Swapchain();
            Swapchain(Context& context, Window& window);

            Swapchain(Swapchain&) = delete;

            void operator=(Swapchain&&);
            Swapchain(Swapchain&&);

            ~Swapchain();

            VkSurfaceFormatKHR getSurfaceFormat();
            std::vector<VkImageView> getImageViews();
        private:
            void createWindowSurface(Window& window);
            void createSwapchain();
            VkSurfaceFormatKHR selectImageFormat();
            void createImageViews(Window& window);


            VkSwapchainKHR vkSwapchain;
            VkSurfaceKHR surface;
            VkSurfaceFormatKHR surfaceFormat;

            std::vector<ImageView> imageViews;
            std::vector<VkImageView> imageViewHandles;

            Context* context;

            const uint32_t PREFERRED_IMAGE_COUNT = 2; 

    };
}