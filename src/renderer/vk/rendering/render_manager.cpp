#include "render_manager.hpp"

namespace Sierra::vlk {

    RenderManager::RenderManager() {

    }

    RenderManager::RenderManager(Context& context, TaskManager& taskManager, MemoryManager& memManager, Info info): context(&context),
     shaderLoader(context, taskManager) {
        createMainPass(info, memManager);
        createGraphicsPipelines(info);

    }
    
    void RenderManager::createMainPass(Info& info, MemoryManager& memManager) {
        RenderPass::Info passInfo{};

        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = info.swapchain->getSurfaceFormat().format;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription mainSubpass{};
        mainSubpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        mainSubpass.colorAttachmentCount = 1;
        mainSubpass.pColorAttachments = &colorAttachmentRef;

        VkSubpassDependency extDependency{};
        extDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        extDependency.dstSubpass = 0;
        extDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        extDependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        extDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        extDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        passInfo.attachments = {colorAttachment};
        passInfo.dependencies = {extDependency};
        passInfo.subpasses = {mainSubpass};
        passInfo.hasDepth = true;
        passInfo.imageViews = info.swapchain->getImageViews();
        passInfo.width = info.swapchain->getWinRes().w;
        passInfo.height = info.swapchain->getWinRes().h;

        mainPass = RenderPass(*context, memManager, passInfo);
    }

    void RenderManager::createGraphicsPipelines(Info& info) {

        VkVertexInputBindingDescription b0{};
        b0.binding = 0;
        b0.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        b0.stride = 24;

        VkVertexInputBindingDescription b1{};
        b1.binding = 1;
        b1.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
        b1.stride = 36;

        VkVertexInputAttributeDescription attr0{}; //vertices
        attr0.binding = 0;
        attr0.format = VK_FORMAT_R32G32B32_SFLOAT;
        attr0.location = 0;
        attr0.offset = 0;

        VkVertexInputAttributeDescription attr4{}; // texcoordsw
        attr4.binding = 0;
        attr4.format = VK_FORMAT_R32G32B32_SFLOAT; // NOT VEC2???
        attr4.location = 4;
        attr4.binding = 0;

        VkVertexInputAttributeDescription attr1{}; //pos
        attr1.binding = 1;
        attr1.format = VK_FORMAT_R32G32B32_SFLOAT;
        attr1.location = 1;
        attr1.offset = 0;

        VkVertexInputAttributeDescription attr2{}; //rot
        attr2.binding = 1;
        attr2.format = VK_FORMAT_R32G32B32_SFLOAT;
        attr2.location = 2;
        attr2.offset = 12;

        VkVertexInputAttributeDescription attr3{}; //scale
        attr3.binding = 1;
        attr3.format = VK_FORMAT_R32G32B32_SFLOAT;
        attr3.location = 3;
        attr3.offset = 24;

        std::array<VkVertexInputAttributeDescription, 5> inputAttrs = {attr0, attr1, attr2, attr3, attr4};
        std::array<VkVertexInputBindingDescription, 2> inputBinds = {b0, b1};

        VkPipelineVertexInputStateCreateInfo vertexInputState{};
        vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputState.pVertexAttributeDescriptions = inputAttrs.data();
        vertexInputState.vertexAttributeDescriptionCount = inputAttrs.size();

        vertexInputState.pVertexBindingDescriptions = inputBinds.data();
        vertexInputState.vertexBindingDescriptionCount = inputBinds.size();

        GraphicsPipeline::PipelineInfo pipelineInfo {};    
        pipelineInfo.inputState = vertexInputState;
        //pipelineInfo.layout set in ShaderLoader
        //pipelineInfo.stages // ditto
        pipelineInfo.renderPass = &mainPass;
        pipelineInfo.subpass = 0;

        for(Renderer::ShaderPair& pair : info.shaders) {
            std::string vertSrc = pair.first.substr(0, pair.first.find_last_of('.'));
            std::string fragSrc = pair.second.substr(0, pair.second.find_last_of('.')); // remove last extension
            //so for e.g. instead of shader.vert.spv it will be shader.vert
            shaderLoader.loadPack(pipelineInfo, {pair.first, vertSrc}, {pair.second, fragSrc});
        }
    }
}