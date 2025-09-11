#pragma once

#include <array>
#include <cstdlib>

#include <vulkan/vulkan.h>

#include "descriptor.hpp"
#include "vk/util.hpp"

namespace Sierra::vlk {
    class DescriptorPool {
        public:
            DescriptorPool();
            DescriptorPool(std::array<size_t, SIERRA_VLK_DESCRIPTOR_TYPE_COUNT> sizes);

            DescriptorPool(DescriptorPool&&);
            void operator=(DescriptorPool&&);

            ~DescriptorPool();

        private:
            void createPool(std::array<size_t, SIERRA_VLK_DESCRIPTOR_TYPE_COUNT> sizes);

            VkDescriptorPool pool;
    };
}