#include "pipeline_layout.hpp"

namespace Sierra::vlk {

    PipelineLayout::Layouts_t PipelineLayout::layouts = {};


    VkPipelineLayout PipelineLayout::getLayout(Context& context, VkDescriptorSetLayout setLayout, std::vector<VkPushConstantRange>& pushConstantRanges) {
        uint64_t hash = getHash(context, setLayout, pushConstantRanges);

        auto it = layouts[context.device->getDevice()].find(hash);

        if (it != layouts[context.device->getDevice()].end())
            return it->second;

        VkPipelineLayout layout = createLayout(context, setLayout, pushConstantRanges);        

        layouts[context.device->getDevice()][hash] = layout;

        return layout;
    }

    uint64_t PipelineLayout::getHash(Context& context, VkDescriptorSetLayout setLayout, std::vector<VkPushConstantRange>& pushConstantRanges) {
        uint64_t hash = (uint64_t)context.device->getDevice();

        hash = combineHashes(hash, (uint64_t)setLayout);

        for (VkPushConstantRange& range : pushConstantRanges) {
            hash = combineHashes(hash, range.offset);
            hash = combineHashes(hash, range.size);
            hash = combineHashes(hash, range.stageFlags);
        }

        return hash;
    }

    VkPipelineLayout PipelineLayout::createLayout(Context& context, VkDescriptorSetLayout setLayout, std::vector<VkPushConstantRange>& pushConstantRanges) {
        VkPipelineLayout layout;

        VkPipelineLayoutCreateInfo layoutInfo{};

        layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

        layoutInfo.setLayoutCount = 1;
        layoutInfo.pSetLayouts = &setLayout;

        layoutInfo.pushConstantRangeCount = pushConstantRanges.size();
        layoutInfo.pPushConstantRanges = pushConstantRanges.data();

        VK_ERR(vkCreatePipelineLayout(context.device->getDevice(), &layoutInfo, nullptr, &layout));

        return layout;
    }

    void PipelineLayout::destroy(Context& context) {
        for (auto layout : layouts[context.device->getDevice()]) {
            vkDestroyPipelineLayout(context.device->getDevice(), layout.second, nullptr);
        }
    }
}