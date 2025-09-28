#pragma once

#include "vk/core/context.hpp"
#include "command_pool.hpp"

namespace Sierra::vlk {
    class CommandBuffer {
        public:
            CommandBuffer();
            CommandBuffer(CommandPool& pool, VkCommandBufferLevel level);

            CommandBuffer(CommandBuffer&) = delete;
            CommandBuffer(CommandBuffer&&);

            void operator=(CommandBuffer&&);

            ~CommandBuffer();

            bool canReset();
            void reset();

            void begin(VkCommandBufferInheritanceInfo *inheritanceInfo);
            void end();

            VkCommandBuffer getCommandBuffer();
        private:
            VkCommandBuffer buffer;
            VkCommandBufferLevel level;
            CommandPool* pool; // while keeping the command pool as a reference does prevent the command pool
            // from being moved, it allows us in the future for the command pool to potentially manage command buffers
            //more thightly, i.e. batching creation/destruction together
    };
}