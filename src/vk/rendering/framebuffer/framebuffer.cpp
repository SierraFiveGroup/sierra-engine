#include "framebuffer.hpp"

namespace Sierra::vlk {
    Framebuffer::Framebuffer(): framebuffer(), context() {

    }

    Framebuffer::Framebuffer(Context& context, Info info): context(&context) {
        createFramebuffer(info);
    }

    void Framebuffer::createFramebuffer(Info& info) {
        VkFramebufferCreateInfo framebufferInfo{};

        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = info.renderPass;
        framebufferInfo.attachmentCount = info.attachments.size();
        framebufferInfo.pAttachments = info.attachments.data();
        framebufferInfo.width = info.width;
        framebufferInfo.height = info.height;
        framebufferInfo.layers = 1;

        VK_ERR(vkCreateFramebuffer(context->device->getDevice(), &framebufferInfo,
                nullptr, &framebuffer));
    }

    Framebuffer::Framebuffer(Framebuffer&& other) {
        framebuffer = other.framebuffer;
        context = other.context;

        other.context = nullptr;
        other.framebuffer = VK_NULL_HANDLE;
    }

    void Framebuffer::operator=(Framebuffer&& other) {
        framebuffer = other.framebuffer;
        context = other.context;

        other.context = nullptr;
        other.framebuffer = VK_NULL_HANDLE;
    }

    Framebuffer::~Framebuffer() {
        if (framebuffer)
            vkDestroyFramebuffer(context->device->getDevice(), framebuffer, nullptr);
    }
}