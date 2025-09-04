#pragma once

#include "vulkan/vulkan.h"

#include "core/instance/instance.hpp"
#include "core/device/device.hpp"
#include "core/context.hpp"
#include "rendering/swapchain/swapchain.hpp"

namespace Sierra::vk {
    class Vulkan {
        public:
            Vulkan(Window& window);

            Vulkan(Vulkan&) = delete;
            Vulkan(Vulkan&&) = delete;

        private:
            Context ctx;

            Instance instance;
            Device device;
            Swapchain swapchain;
    };
}

