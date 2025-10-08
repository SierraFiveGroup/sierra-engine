#pragma once

#include <vector>

#include "../mem.hpp"

#include <vulkan/vulkan.h>


namespace Sierra::vlk {
    class Buffer { 
        public:
            enum class Type {
                DEVICE_LOCAL,
                HOST_LOCAL
            };

            enum class Usage {
                NONE,
                UNIFORM,
                STORAGE,
                INDEX,
                VERTEX
            };

            struct Info {
                Type type;
                Usage usage;
                VkDeviceSize size;
                VkBufferUsageFlags additionalUsageFlags;
                std::vector<uint32_t> queueFamilyIndices;
            };

            Buffer();
            Buffer(Context& context, Info& info);

            size_t getSize();

            void* map();
            void unmap();
            void copyToBuff(uint8_t* src, size_t srcSize);

            VkBuffer getBuff();
        private:
            void createBuff(Context& context, Info& info);

            Mem mem;
            VkBuffer buff;

            Type type;
    };
}