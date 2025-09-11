#include "descriptor_pool.hpp"

namespace Sierra::vlk {
    void DescriptorPool::createPool(std::array<size_t, SIERRA_VLK_DESCRIPTOR_TYPE_COUNT> sizes) {
        VkDescriptorPoolSize poolSizes[SIERRA_VLK_DESCRIPTOR_TYPE_COUNT];
        for(int i = 0; i < sizes.size(); i++) {
            poolSizes[i].type = (VkDescriptorType)i;
            poolSizes[i].descriptorCount = sizes[i];
        }


        VkDescriptorPoolCreateInfo poolInfo{};

        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = SIERRA_VLK_DESCRIPTOR_TYPE_COUNT;
        poolInfo.pPoolSizes = poolSizes;
    }
}