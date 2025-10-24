#include "render_pass.hpp"

namespace Sierra::vlk {

    RenderPass::RenderPass(): vkRenderPass(VK_NULL_HANDLE), context(nullptr),
     width(), height() {

    }

    RenderPass::RenderPass(Context& context, Info& info): vkRenderPass(VK_NULL_HANDLE), context(&context),
     width(info.width), height(info.height) {
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
        VK_ASSERT(!info.imageViews.empty());

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

    VkRenderPassBeginInfo RenderPass::getBeginInfo(uint32_t framebufferIndex) {
        static VkClearValue clearVal{};
        clearVal.color.uint32[0] = 1;
        clearVal.color.uint32[1] = 1;
        clearVal.color.uint32[2] = 1;
        clearVal.color.uint32[3] = 1;
        clearVal.depthStencil.depth = 0.0;
        clearVal.depthStencil.stencil = 0.0;

        VkRenderPassBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        beginInfo.renderPass = vkRenderPass;
        beginInfo.framebuffer = framebuffers[framebufferIndex].getFramebuffer();
        beginInfo.renderArea = {{0, 0}, {width, height}};
        beginInfo.clearValueCount = 1;
        beginInfo.pClearValues = &clearVal;

        return beginInfo;
    }

    VkRenderPass RenderPass::getRenderPass() {
        return vkRenderPass;
    }

    RenderPass::RenderPass(RenderPass&& other) {
        context = other.context;
        vkRenderPass = other.vkRenderPass;
        width = other.width;
        height = other.height;
        framebuffers = std::move(other.framebuffers);

        other.context = nullptr;
        other.vkRenderPass = VK_NULL_HANDLE;
    }
    
    void RenderPass::operator=(RenderPass&& other) {
        context = other.context;
        vkRenderPass = other.vkRenderPass;
        width = other.width;
        height = other.height;
        framebuffers = std::move(other.framebuffers);

        other.context = nullptr;
        other.vkRenderPass = VK_NULL_HANDLE;
    }

    RenderPass::~RenderPass() {
        if(vkRenderPass)
            vkDestroyRenderPass(context->device->getDevice(), vkRenderPass, nullptr);
    }
}