#pragma once

#include <array>
#include <vector>

#include "descriptor_layout.hpp"
#include "descriptor_pool.hpp"

namespace Sierra::vlk {
    class DescriptorSet {
        public:
            DescriptorSet();
            DescriptorSet(Context& context, std::vector<Descriptor*>& descriptors, DescriptorPool& pool);

            DescriptorSet(DescriptorSet&&);
            void operator=(DescriptorSet&&);

            ~DescriptorSet();
        private:

            void createSet(std::vector<Descriptor*>& descriptors, DescriptorPool& pool);

            VkDescriptorSet set;
            Context* context;

            std::array<std::vector<Descriptor*>, SIERRA_VLK_DESCRIPTOR_TYPE_COUNT> descriptorHandles;
    };
}