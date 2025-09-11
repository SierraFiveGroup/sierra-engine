#include "descriptor_layout.hpp"

uint64_t combineHashes(uint64_t h1, uint64_t h2) {
    h1 ^= h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2); // stolen from boost
    return h1;
}

namespace Sierra::vlk {

    DescriptorLayout::Layouts_t layouts = {};

    void DescriptorLayout::destroy(Context& context) {
        layouts.erase(context.device->getDevice());
    }

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

        VkDescriptorSetLayout layout;

        VK_ERR(vkCreateDescriptorSetLayout(context.device->getDevice(), &layoutInfo, nullptr, &layout));

        return layout;
    }


} 