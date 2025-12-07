#pragma once

#include <string>
#include <vulkan/vulkan.h>

#include "vk/res/mem/buffer/buffer.hpp"
#include "vk/res/mem/image/image.hpp"
#include "vk/res/loadable/texture/vk_texture.hpp"

#define SIERRA_VLK_DESCRIPTOR_TYPE_COUNT 10

namespace Sierra::vlk {
    class Descriptor {
        friend class DescriptorSet;
        friend class Shader;

        public:
            Descriptor();
            Descriptor(std::string name, VkDescriptorType type);

            std::string getName();
            VkDescriptorType getType();
            uint32_t getIndex();

            VkWriteDescriptorSet getWrite();
            VkWriteDescriptorSet getWriteBuffer(Buffer& buffer);
            VkWriteDescriptorSet getWriteImage(VlkTexture& tex);
        
        protected:
            void setIndex(uint32_t index);
            void setDescriptorSet(VkDescriptorSet set);

        private:
            std::string name;
            uint32_t index;
            VkDescriptorType type;
            VkDescriptorSet set;
    };
}