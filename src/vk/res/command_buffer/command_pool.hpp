#pragma once

#include "vk/core/context.hpp"

#include <vulkan/vulkan.h>

namespace Sierra::vlk {
    class CommandPool {
        friend class CommandBuffer;
        public:
            CommandPool();
            CommandPool(Context& context, uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags);

            CommandPool(CommandPool&) = delete;

            CommandPool(CommandPool&&);
            void operator=(CommandPool&&);

            ~CommandPool();
        protected: // meant for the command buffrer
            VkCommandBuffer allocCmdBuffer(VkCommandBufferLevel level);
            void freeCmdBuffer(VkCommandBuffer buffer);
            bool canResetCmdBuffer();
        private:
            void createPool(Context& context, uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags);

            VkCommandPool commandPool;
            Context* context;

            bool resetCmdBuffer;
    };
}