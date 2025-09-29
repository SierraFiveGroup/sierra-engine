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

            struct Info {
                VkDeviceSize size;
                VkBufferUsageFlags usage;
                std::vector<uint32_t> queueFamilyIndices;
                Type type;
            };

            Buffer();
            Buffer(Context& context, Info& info);

            size_t getSize();

            void* map();
            void unmap();
            void copyToBuff(uint8_t* src, size_t srcSize);
        private:
            void createBuff(Context& context, Info& info);

            Mem mem;
            VkBuffer buff;

            Type type;
    };
}