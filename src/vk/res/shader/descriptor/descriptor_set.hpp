#pragma once

#include "descriptor_layout.hpp"
#include "descriptor_pool.hpp"

namespace Sierra::vlk {
    class DescriptorSet {
        public:
            DescriptorSet();
            DescriptorSet(Context& context, std::vector<VkDescriptorSetLayoutBinding>& bindings, DescriptorPool& pool);

            DescriptorSet(DescriptorSet&&);
            void operator=(DescriptorSet&&);

            ~DescriptorSet();
        private:

            void createSet(std::vector<VkDescriptorSetLayoutBinding>& bindings, DescriptorPool& pool);

            VkDescriptorSet set;
            Context* context;
    };
}