#pragma once

#include <array>
#include <cstdlib>

#include <vulkan/vulkan.h>

#include "descriptor.hpp"
#include "util.hpp"
#include "core/context.hpp"

namespace Sierra::vlk {
    class DescriptorPool {
        public:
            DescriptorPool();
            DescriptorPool(Context& context, std::array<size_t, SIERRA_VLK_DESCRIPTOR_TYPE_COUNT> sizes);

            DescriptorPool(DescriptorPool&&);
            void operator=(DescriptorPool&&);

            ~DescriptorPool();

            void allocateSets(std::vector<VkDescriptorSetLayout>& layouts, std::vector<VkDescriptorSet>& sets);

            VkDescriptorPool getPool();

        private:
            void createPool(std::array<size_t, SIERRA_VLK_DESCRIPTOR_TYPE_COUNT> sizes);

            VkDescriptorPool pool;
            Context* context;
    };
}