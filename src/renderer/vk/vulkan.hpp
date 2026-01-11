#pragma once

#include "vulkan/vulkan.h"
#include "renderer.hpp"

#include "core/instance/instance.hpp"
#include "core/device/device.hpp"
#include "core/context.hpp"
#include "rendering/swapchain/swapchain.hpp"
#include "scene/scene.hpp"
#include "res/mem/mem.hpp"
#include "res/loadable/model/vk_model.hpp"

namespace Sierra::vlk {
    class Vulkan {
        public:
            Vulkan();
            Vulkan(Window& window);

            Vulkan(Vulkan&) = delete;
            Vulkan(Vulkan&&) = delete;

            ~Vulkan();

            std::vector<Res::ResourceAny> loadResources(ResourceManager::LoadPacket loadInfo);

            void cleanup();

            Scene createScene();
            Context& getContext();
            Swapchain& getSwapchain();
        private:
            Instance instance;
            Device device;
            Context context;

            Swapchain swapchain;

            TaskManager taskManager;
            MemLoader memLoader;
            MemoryManager memManager;
    };
}

