#include "render_pass.hpp"

namespace Sierra::vlk {

    RenderPass::RenderPass(): vkRenderPass(VK_NULL_HANDLE), context(nullptr),
     width(), height(), depth() {

    }

    RenderPass::RenderPass(Context& context, MemoryManager& memoryManager, Info& info): vkRenderPass(VK_NULL_HANDLE), context(&context),
     width(info.width), height(info.height), depth(info.hasDepth) {
        if(info.hasDepth)
            createDepthResources(context, memoryManager);

        createRenderPass(info);
        createFramebuffers(info);

    }
    
    void RenderPass::createRenderPass(Info& info) {

        VkAttachmentReference depthRef = {};
        if(depth) {
            depthRef = appendDepthAttachment(info.attachments);

            for(VkSubpassDescription& subpass : info.subpasses) {
                subpass.pDepthStencilAttachment = &depthRef;
            }
        }

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
            framebufferInfo.attachments = { imageView, depthImage.getView() };

            framebuffers.emplace_back(*context, framebufferInfo); 
        }
    }

    VkRenderPassBeginInfo RenderPass::getBeginInfo(uint32_t framebufferIndex) {
        static VkClearValue colorClear{};
        colorClear.color.uint32[0] = 1;
        colorClear.color.uint32[1] = 1;
        colorClear.color.uint32[2] = 1;
        colorClear.color.uint32[3] = 1;

        static VkClearValue depthClear{};
        depthClear.depthStencil.depth = 1.0f;

        static VkClearValue clearValues[] = {colorClear, depthClear};

        int clearValueCount = 1;
        if(depth)
            clearValueCount = 2;

        VkRenderPassBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        beginInfo.renderPass = vkRenderPass;
        beginInfo.framebuffer = framebuffers[framebufferIndex].getFramebuffer();
        beginInfo.renderArea = {{0, 0}, {width, height}};
        beginInfo.clearValueCount = clearValueCount;
        beginInfo.pClearValues = clearValues;

        return beginInfo;
    }
    
    void RenderPass::createDepthResources(Context& context, MemoryManager& memoryManager) {
        Image::Info imageInfo{}; 

        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.format = VK_FORMAT_D32_SFLOAT;
        imageInfo.extent = {width, height, 1};
        imageInfo.mipLevels = 1;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.queueFamilyIndices = {context.device->getQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT)};
        imageInfo.layout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
        imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        imageInfo.type =  Mem::Type::PREFER_DEVICE;

        depthImage = Image(context, memoryManager, imageInfo);
    }

    VkAttachmentReference RenderPass::appendDepthAttachment(std::vector<VkAttachmentDescription>& attachments) {
        VkAttachmentDescription depthAttachment{};
        depthAttachment.format = VK_FORMAT_D32_SFLOAT;
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        attachments.push_back(depthAttachment);

        VkAttachmentReference attachmentRef{};
        attachmentRef.attachment = attachments.size() - 1;
        attachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        return attachmentRef;
    }

    VkRenderPass RenderPass::getRenderPass() {
        return vkRenderPass;
    }

    bool RenderPass::hasDepth() {
        return depth;
    }

    RenderPass::RenderPass(RenderPass&& other) {
        context = other.context;
        vkRenderPass = other.vkRenderPass;
        width = other.width;
        height = other.height;
        framebuffers = std::move(other.framebuffers);
        depth = other.depth;

        other.context = nullptr;
        other.vkRenderPass = VK_NULL_HANDLE;
    }
    
    void RenderPass::operator=(RenderPass&& other) {
        context = other.context;
        vkRenderPass = other.vkRenderPass;
        width = other.width;
        height = other.height;
        framebuffers = std::move(other.framebuffers);
        depth = other.depth;

        other.context = nullptr;
        other.vkRenderPass = VK_NULL_HANDLE;
    }

    RenderPass::~RenderPass() {
        if(vkRenderPass)
            vkDestroyRenderPass(context->device->getDevice(), vkRenderPass, nullptr);
    }
}