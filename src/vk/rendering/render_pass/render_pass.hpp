#pragma once

#include <vector>

#include <vulkan/vulkan.hpp>

#include "../../core/context.hpp"
#include "../../util.hpp"

namespace Sierra::vlk {
    class RenderPass {
        public:
            struct CreateInfo {
                std::vector<VkAttachmentDescription> attachments;
                std::vector<VkSubpassDescription> subpasses;
                std::vector<VkSubpassDependency> dependencies;
            };

            RenderPass();
            RenderPass(Context& context, CreateInfo& info);

            RenderPass(RenderPass&) = delete;

            RenderPass(RenderPass&&);
            void operator=(RenderPass&&);

            ~RenderPass();

            VkRenderPass getRenderPass();
        private:
            void createRenderPass(CreateInfo& info);

            VkRenderPass vkRenderPass;

            Context* context;
    };

}