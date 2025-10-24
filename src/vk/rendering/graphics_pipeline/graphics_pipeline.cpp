#include "graphics_pipeline.hpp"

namespace Sierra::vlk {
    GraphicsPipeline::Pipelines_t GraphicsPipeline::pipelines = {};

    VkPipeline GraphicsPipeline::getPipeline(Context& context, PipelineInfo& info) {
        uint64_t hash = getHash(context, info);

        auto it = pipelines[context.device->getDevice()].find(hash);

        if(it != pipelines[context.device->getDevice()].end())
            return it->second;

        
        VkPipeline pipeline = createPipeline(context, info);

        pipelines[context.device->getDevice()][hash] = pipeline;

        return pipeline;
    }

    VkPipeline GraphicsPipeline::createPipeline(Context& context, PipelineInfo& info) {
        VkPipeline pipeline = VK_NULL_HANDLE;

        VkPipelineInputAssemblyStateCreateInfo assemblyInfo{};
        assemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        assemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; 
        assemblyInfo.primitiveRestartEnable = VK_FALSE;

        VkPipelineTessellationStateCreateInfo tesselationInfo{};
        tesselationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
        tesselationInfo.patchControlPoints = 1;

        VkPipelineViewportStateCreateInfo viewportInfo{};
        viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportInfo.viewportCount = 1;
        viewportInfo.scissorCount = 1;

        VkPipelineRasterizationStateCreateInfo rasterizationInfo{};
        rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL; // TODO add line option for debugging
        rasterizationInfo.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizationInfo.frontFace =  VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizationInfo.lineWidth = 1.0f;
        rasterizationInfo.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisampleInfo{};
        multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT; // TODO make configurable

        VkPipelineDepthStencilStateCreateInfo depthStencilInfo{};
        depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO; 
        depthStencilInfo.depthTestEnable = VK_TRUE; 
        depthStencilInfo.minDepthBounds = 0.0;
        depthStencilInfo.maxDepthBounds = 1.0;

        VkPipelineColorBlendAttachmentState colorState{};
        colorState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
         VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_R_BIT;
        colorState.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo blendInfo{};
        blendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        blendInfo.logicOpEnable = VK_TRUE;
        blendInfo.logicOp = VK_LOGIC_OP_AND;
        blendInfo.pAttachments = &colorState;
        blendInfo.attachmentCount = 1;

        VkDynamicState dynamicStates[] = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };

        VkPipelineDynamicStateCreateInfo dynamicInfo{};
        dynamicInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicInfo.dynamicStateCount = sizeof(dynamicStates) / sizeof(VkDynamicState);
        dynamicInfo.pDynamicStates = dynamicStates;

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = info.stages.size();
        pipelineInfo.pStages = info.stages.data();
        pipelineInfo.pVertexInputState = &info.inputState;
        pipelineInfo.pInputAssemblyState = &assemblyInfo;
        pipelineInfo.pTessellationState = &tesselationInfo;
        pipelineInfo.pViewportState = &viewportInfo;
        pipelineInfo.pRasterizationState = &rasterizationInfo;
        pipelineInfo.pMultisampleState = &multisampleInfo;
        pipelineInfo.pDepthStencilState = &depthStencilInfo;
        pipelineInfo.pColorBlendState = &blendInfo;
        pipelineInfo.pDynamicState = &dynamicInfo;
        pipelineInfo.layout = info.layout;
        pipelineInfo.renderPass = info.renderPass->getRenderPass();
        pipelineInfo.subpass = info.subpass;
        pipelineInfo.basePipelineHandle = info.basePipeline;
        pipelineInfo.basePipelineIndex = info.basePipelineIndex;

        VK_ERR(vkCreateGraphicsPipelines(context.device->getDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline)); // TODO Pipeline caching

        return pipeline;
    }

    uint64_t GraphicsPipeline::getHash(Context& context, PipelineInfo& info) { //actual dogshit
        uint64_t hash = (uint64_t)context.device->getDevice(); //basically cram all the info possible
        //and I aint hashing image views since thats stupid

        hash = combineHashes((uint64_t)info.renderPass->getRenderPass(), hash);
        hash = combineHashes(info.subpass, hash);
        hash = combineHashes(info.basePipelineIndex, hash);
        hash = combineHashes((uint64_t)info.basePipeline, hash);
        hash = combineHashes((uint64_t)info.layout, hash);

        for(int i = 0; i < info.inputState.vertexAttributeDescriptionCount; i++) {
            hash = combineHashes(hash, info.inputState.pVertexAttributeDescriptions[i].binding);
            hash = combineHashes(hash, info.inputState.pVertexAttributeDescriptions[i].format);
            hash = combineHashes(hash, info.inputState.pVertexAttributeDescriptions[i].location);
            hash = combineHashes(hash, info.inputState.pVertexAttributeDescriptions[i].offset);
        }

        for(int i = 0; i < info.inputState.vertexBindingDescriptionCount; i++) {
            hash = combineHashes(hash, info.inputState.pVertexBindingDescriptions[i].binding);
            hash = combineHashes(hash, info.inputState.pVertexBindingDescriptions[i].inputRate);
            hash = combineHashes(hash, info.inputState.pVertexBindingDescriptions[i].stride);
        }

        for(VkPipelineShaderStageCreateInfo& stage : info.stages) {
            hash = combineHashes(hash, stage.flags);
            hash = combineHashes(hash, (uint64_t)stage.module);
            hash = combineHashes(hash, stage.stage);
        }

        return hash;
    }

    void GraphicsPipeline::destroy(Context& context) {
        for (auto pipeline : pipelines[context.device->getDevice()]) {
            vkDestroyPipeline(context.device->getDevice(), pipeline.second, nullptr);
        }
    }
}