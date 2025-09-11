#include "descriptor_pool.hpp"

namespace Sierra::vlk {
    DescriptorPool::DescriptorPool(): pool(VK_NULL_HANDLE), context(nullptr) {

    }

    DescriptorPool::DescriptorPool(Context& context, std::array<size_t, SIERRA_VLK_DESCRIPTOR_TYPE_COUNT> sizes): pool(VK_NULL_HANDLE), context(&context)  {
        createPool(sizes);
    }

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
        poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;

        VK_ERR((vkCreateDescriptorPool(context->device->getDevice(), &poolInfo, nullptr, &pool)));
    }

    DescriptorPool::DescriptorPool(DescriptorPool&& other) {
        pool = other.pool;
        context = other.context;

        other.pool = VK_NULL_HANDLE;
        other.context = nullptr;
    }

    void DescriptorPool::operator=(DescriptorPool&& other) {
        pool = other.pool;
        context = other.context;

        other.pool = VK_NULL_HANDLE;
        other.context = nullptr;
    }

    DescriptorPool::~DescriptorPool() {
        if(pool)
            vkDestroyDescriptorPool(context->device->getDevice(), pool, nullptr);
    }
}