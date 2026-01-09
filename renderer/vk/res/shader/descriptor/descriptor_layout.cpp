#include "descriptor_layout.hpp"

namespace Sierra::vlk {

    DescriptorLayout::Layouts_t DescriptorLayout::layouts = {};

    VkDescriptorSetLayout DescriptorLayout::getLayout(Context& context, std::vector<VkDescriptorSetLayoutBinding>& bindings) {
        uint64_t hash = 0;
        for(VkDescriptorSetLayoutBinding& binding : bindings) {
            combineHashes(hash, binding.binding);
            combineHashes(hash, binding.descriptorType);
            combineHashes(hash, binding.descriptorCount);
        }

        auto it = layouts[context.device->getDevice()].find(hash);

        if (it != layouts[context.device->getDevice()].end())
            return it->second;


        VkDescriptorSetLayout layout = createLayout(context, bindings);

        layouts[context.device->getDevice()][hash] = layout;

        return layout;
    }

    VkDescriptorSetLayout DescriptorLayout::createLayout(Context& context, std::vector<VkDescriptorSetLayoutBinding>& bindings) {
        VkDescriptorSetLayoutCreateInfo layoutInfo{};

        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;

        layoutInfo.bindingCount = bindings.size();
        layoutInfo.pBindings = bindings.data();
        
        layoutInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;

        std::vector<VkDescriptorBindingFlags> bindingFlags(bindings.size());
        for(VkDescriptorBindingFlags& flags : bindingFlags) {
            flags = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;
        }

        VkDescriptorSetLayoutBindingFlagsCreateInfo flagsInfo{};
        flagsInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
        flagsInfo.pBindingFlags = bindingFlags.data();
        flagsInfo.bindingCount = bindingFlags.size();

        layoutInfo.pNext = &flagsInfo;

        VkDescriptorSetLayout layout;

        VK_ERR(vkCreateDescriptorSetLayout(context.device->getDevice(), &layoutInfo, nullptr, &layout));

        return layout;
    }

    void DescriptorLayout::destroy(Context& context) {
        for(auto layout : layouts[context.device->getDevice()]) {
            vkDestroyDescriptorSetLayout(context.device->getDevice(), layout.second, nullptr);
        }
    }
} 