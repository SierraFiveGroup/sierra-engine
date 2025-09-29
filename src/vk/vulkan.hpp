#pragma once

#include "vulkan/vulkan.h"

#include "core/instance/instance.hpp"
#include "core/device/device.hpp"
#include "core/context.hpp"
#include "rendering/swapchain/swapchain.hpp"
#include "vk/scene/scene.hpp"
#include "vk/res/mem/mem.hpp"

namespace Sierra::vlk {
    class Vulkan {
        public:
            Vulkan(Window& window);

            Vulkan(Vulkan&) = delete;
            Vulkan(Vulkan&&) = delete;

            ~Vulkan();

            Scene createScene();
            Context& getContext();
            Swapchain& getSwapchain();
        private:
            Context context;

            Instance instance;
            Device device;
            Swapchain swapchain;
    };
}

