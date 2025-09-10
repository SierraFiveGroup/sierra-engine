#include "scene_renderer.hpp"

namespace Sierra::vlk {
    SceneRenderer::SceneRenderer(): mainPass() {

    }

    SceneRenderer::SceneRenderer(Context& context, Swapchain& swapchain) {
        createRenderPass(context, swapchain);
    }

    void SceneRenderer::createRenderPass(Context& context, Swapchain& swapchain) {
        VkAttachmentDescription colorAttachment{};        
        colorAttachment.format = swapchain.getSurfaceFormat().format;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        std::vector<VkAttachmentDescription> attachments = {colorAttachment};
        std::vector<VkAttachmentReference> references = {colorAttachmentRef};
        std::vector<VkSubpassDescription> subpasses = {subpass};

        RenderPass::CreateInfo info = {
            .attachments = attachments,
            .subpasses = subpasses,
            .dependencies = {}
        };

        mainPass = RenderPass(context, info);
    }
}