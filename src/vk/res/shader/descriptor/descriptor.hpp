#pragma once

#include <string>

#include <vulkan/vulkan.h>

#define SIERRA_VLK_DESCRIPTOR_TYPE_COUNT 10

namespace Sierra::vlk {
    class Descriptor {
        public:
            Descriptor();
            Descriptor(std::string name, VkDescriptorType type);

            std::string getName();
            VkDescriptorType getType();

            void setIndex(uint32_t index);
        private:
            std::string name;
            uint32_t index;
            VkDescriptorType type;
    };
}