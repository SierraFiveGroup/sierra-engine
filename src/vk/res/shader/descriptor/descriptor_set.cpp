#include "descriptor_set.hpp"

namespace Sierra::vlk {
    DescriptorSet::DescriptorSet(): set(VK_NULL_HANDLE), context(nullptr) {

    }

    DescriptorSet::DescriptorSet(Context& context, std::vector<VkDescriptorSetLayoutBinding>& bindings, DescriptorPool& pool): context(&context), set(VK_NULL_HANDLE) {
       createSet(bindings, pool); 
    }
    
    
    void DescriptorSet::createSet(std::vector<VkDescriptorSetLayoutBinding>& bindings, DescriptorPool& pool) {
        VkDescriptorSetLayout layoutHandle = DescriptorLayout::getLayout(*context, bindings);

        VkDescriptorSetAllocateInfo allocInfo{};

        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;

        allocInfo.descriptorPool = pool.getPool();

        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &layoutHandle;

        VK_ERR(vkAllocateDescriptorSets(context->device->getDevice(), &allocInfo, &set));
    }

    DescriptorSet::DescriptorSet(DescriptorSet&& other) {
        set = other.set;
        context = other.context;

        other.set = VK_NULL_HANDLE;
        other.context = nullptr;
    }

    void DescriptorSet::operator=(DescriptorSet&& other) {
        set = other.set;
        context = other.context;

        other.set = VK_NULL_HANDLE;
        other.context = nullptr;
    }

    DescriptorSet::~DescriptorSet() {

    }
}