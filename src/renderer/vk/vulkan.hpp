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
#include "res/loadable/loader.hpp"
#include "vk/rendering/render_manager.hpp"

namespace Sierra::vlk {
    class Vulkan {
        public:
            Vulkan();
            Vulkan(Renderer::Configuration conf);

            Vulkan(Vulkan&) = delete;
            Vulkan(Vulkan&&) = delete;

            ~Vulkan();

            std::vector<Res::ResourceAny> loadResources(ResourceManager::_RendererLoadPacket packet);

            void cleanup();

            Scene createScene();
            Context& getContext();
            Swapchain& getSwapchain();
        private:

            void run();

            std::thread runThread;

            Instance instance;
            Device device;
            Context context;


            Loader loader;
            TaskManager* taskManager;

            MemoryManager memManager;
            ResourceManager* resManager;

            Swapchain swapchain;
            RenderManager renderManager;
    };
}

