#pragma once

#include <vector>

#include <vulkan/vulkan.hpp>

#include "../../core/context.hpp"
#include "../../util.hpp"
#include "../framebuffer/framebuffer.hpp"
#include "vk/res/mem/image/image.hpp"
#include "vk/res/mem/image/image_view.hpp"

namespace Sierra::vlk {
    class RenderPass {
        public:
            struct Info {
                std::vector<VkAttachmentDescription> attachments;
                std::vector<VkSubpassDescription> subpasses;
                std::vector<VkSubpassDependency> dependencies;
                std::vector<VkImageView> imageViews;

                uint32_t width;
                uint32_t height;

                bool hasDepth;
            };

            RenderPass();
            RenderPass(Context& context, MemoryManager& memoryManager, Info& info);

            RenderPass(RenderPass&) = delete;

            RenderPass(RenderPass&&);
            void operator=(RenderPass&&);

            ~RenderPass();

            VkRenderPassBeginInfo getBeginInfo(uint32_t framebufferIndex);
            VkRenderPass getRenderPass();
            bool hasDepth(); 
        private:
            void createRenderPass(Info& info);
            void createFramebuffers(Info& info);
            void createDepthResources(Context& context, MemoryManager& memoryManager);

            VkAttachmentReference appendDepthAttachment(std::vector<VkAttachmentDescription>& attachments);

            VkRenderPass vkRenderPass;

            Context* context;

            uint32_t width;
            uint32_t height;
            bool depth;

            Image depthImage;

            std::vector<Framebuffer> framebuffers;
    };

}