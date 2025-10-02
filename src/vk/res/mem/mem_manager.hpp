#pragma once

#include <vulkan/vulkan.hpp>

namespace Sierra::vlk {

    class MemoryManager {
        public:
            struct TransferOp {
                void* src;

                size_t size;
                size_t offset;
            
                VkBuffer buffer;
                VkImage image;
            };

            MemoryManager();

            MemoryManager(MemoryManager&) = delete;
            MemoryManager(MemoryManager&&) = delete;
        private:
    };
}