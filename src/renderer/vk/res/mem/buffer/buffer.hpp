#pragma once

#include <vector>
#include <memory>

#include "../mem.hpp"

#include <vulkan/vulkan.h>


namespace Sierra::vlk {
    class Buffer { 
        public:
            enum class Type {
                HOST_LOCAL,
                DEVICE_LOCAL
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
            VkDescriptorBufferInfo* getDescriptorInfo();
        private:
            void createBuff(Context& context, Info& info);
            void initDescriptorInfo();

            Mem mem;
            VkBuffer buff;

            std::shared_ptr<VkDescriptorBufferInfo> descriptorInfo;

            Type type;
    };
}