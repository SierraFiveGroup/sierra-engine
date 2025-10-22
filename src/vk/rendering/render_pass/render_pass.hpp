#pragma once

#include <vector>

#include <vulkan/vulkan.hpp>

#include "../../core/context.hpp"
#include "../../util.hpp"
#include "../framebuffer/framebuffer.hpp"

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
            };

            RenderPass();
            RenderPass(Context& context, Info& info);

            RenderPass(RenderPass&) = delete;

            RenderPass(RenderPass&&);
            void operator=(RenderPass&&);

            ~RenderPass();

            VkRenderPass getRenderPass();
        private:
            void createRenderPass(Info& info);
            void createFramebuffers(Info& info);

            VkRenderPass vkRenderPass;

            Context* context;

            std::vector<Framebuffer> framebuffers;
    };

}