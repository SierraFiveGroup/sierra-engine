#include "descriptor_pool.hpp"

namespace Sierra::vlk {
    DescriptorPool::DescriptorPool(): pool(VK_NULL_HANDLE), context(nullptr) {

    }

    DescriptorPool::DescriptorPool(Context& context, std::array<size_t, SIERRA_VLK_DESCRIPTOR_TYPE_COUNT> sizes): pool(VK_NULL_HANDLE), context(&context)  {
        createPool(sizes);
    }

    void DescriptorPool::createPool(std::array<size_t, SIERRA_VLK_DESCRIPTOR_TYPE_COUNT> sizes) {
        std::vector<VkDescriptorPoolSize> poolSizes;
        for(int i = 0; i < sizes.size(); i++) {
            if(!sizes[i]) continue;

            VkDescriptorPoolSize size{};
            size.type = (VkDescriptorType)i;
            size.descriptorCount = sizes[i];

            poolSizes.push_back(size);
        }


        VkDescriptorPoolCreateInfo poolInfo{};

        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = poolSizes.size();
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
        poolInfo.maxSets = 1; // TODO valid???

        VK_ERR((vkCreateDescriptorPool(context->device->getDevice(), &poolInfo, nullptr, &pool)));
    }

    void DescriptorPool::allocateSets(std::vector<VkDescriptorSetLayout>& layouts, std::vector<VkDescriptorSet>& sets) {
        VkDescriptorSetAllocateInfo allocInfo{};

        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = pool;

        allocInfo.descriptorSetCount = layouts.size();
        allocInfo.pSetLayouts = layouts.data();

        VK_ERR(vkAllocateDescriptorSets(context->device->getDevice(), nullptr, sets.data()));
    }

    VkDescriptorPool DescriptorPool::getPool() {
        return pool;
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