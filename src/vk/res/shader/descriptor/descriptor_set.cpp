#include "descriptor_set.hpp"

namespace Sierra::vlk {
    DescriptorSet::DescriptorSet(): set(VK_NULL_HANDLE), context(nullptr) {

    }

    DescriptorSet::DescriptorSet(Context& context, std::vector<Descriptor*>& descriptors, DescriptorPool& pool): context(&context), set(VK_NULL_HANDLE) {
       createSet(descriptors, pool); 
    }
    
    
    void DescriptorSet::createSet(std::vector<Descriptor*>& descriptors, DescriptorPool& pool) {
        std::array<size_t, SIERRA_VLK_DESCRIPTOR_TYPE_COUNT> sizes {};
        std::vector<VkDescriptorSetLayoutBinding> bindings {};

        for(Descriptor* descriptor : descriptors) {
            sizes[descriptor->getType()]++;

            descriptorHandles[descriptor->getType()].push_back(descriptor);
         //   descriptor->setIndex(descriptorHandles[descriptor->getType()].size() - 1);

            VkDescriptorSetLayoutBinding binding{};

            binding.binding = descriptor->getIndex(); // descriptor index guys
            binding.descriptorType = descriptor->getType();
            binding.descriptorCount = 1;
            binding.stageFlags = VK_SHADER_STAGE_ALL;

            bindings.push_back(binding);
        }

        layout = DescriptorLayout::getLayout(*context, bindings);

        VkDescriptorSetAllocateInfo allocInfo{};

        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;

        allocInfo.descriptorPool = pool.getPool();

        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &layout;

        VK_ERR(vkAllocateDescriptorSets(context->device->getDevice(), &allocInfo, &set));

        for(Descriptor* descriptor : descriptors) {
            descriptor->setDescriptorSet(set);
        }
    }

    VkDescriptorSet DescriptorSet::getSet() {
        return set;
    }

    VkDescriptorSetLayout DescriptorSet::getLayout() {
        return layout;
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