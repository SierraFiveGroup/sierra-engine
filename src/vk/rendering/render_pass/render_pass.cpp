#include "render_pass.hpp"

namespace Sierra::vlk {

    RenderPass::RenderPass(): vkRenderPass(VK_NULL_HANDLE), context(nullptr) {

    }

    RenderPass::RenderPass(Context& context, Info& info): vkRenderPass(VK_NULL_HANDLE), context(&context) {
        createRenderPass(info);
        createFramebuffers(info);
    }
    
    void RenderPass::createRenderPass(Info& info) {
        VkRenderPassCreateInfo passInfo{};

        passInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

        passInfo.attachmentCount = info.attachments.size();
        passInfo.pAttachments = info.attachments.data();

        passInfo.dependencyCount = info.dependencies.size();
        passInfo.pDependencies = info.dependencies.data();

        passInfo.subpassCount = info.subpasses.size();
        passInfo.pSubpasses = info.subpasses.data();

        VK_ERR(vkCreateRenderPass(context->device->getDevice(), &passInfo, nullptr, &vkRenderPass));
    }

    void RenderPass::createFramebuffers(Info& info) {
        Framebuffer::Info framebufferInfo{};
        framebufferInfo.width = info.width;
        framebufferInfo.height = info.height;
        framebufferInfo.renderPass = vkRenderPass;

        framebuffers.reserve(info.imageViews.size());
        for (VkImageView& imageView : info.imageViews) {
            framebufferInfo.attachments = {imageView};

            framebuffers.emplace_back(*context, framebufferInfo); 
        }
    }

    VkRenderPass RenderPass::getRenderPass() {
        return vkRenderPass;
    }

    RenderPass::RenderPass(RenderPass&& other) {
        context = other.context;
        vkRenderPass = other.vkRenderPass;

        other.context = nullptr;
        other.vkRenderPass = VK_NULL_HANDLE;
    }
    
    void RenderPass::operator=(RenderPass&& other) {
        context = other.context;
        vkRenderPass = other.vkRenderPass;

        other.context = nullptr;
        other.vkRenderPass = VK_NULL_HANDLE;
    }

    RenderPass::~RenderPass() {
        if(vkRenderPass)
            vkDestroyRenderPass(context->device->getDevice(), vkRenderPass, nullptr);
    }
}