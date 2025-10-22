#pragma once

#include "vk/core/context.hpp"

namespace Sierra::vlk {
    class Framebuffer {
        public:
            struct Info {
                VkRenderPass renderPass;
                std::vector<VkImageView> attachments; 
            
                uint32_t width;
                uint32_t height;
            };

            Framebuffer();
            Framebuffer(Context& context, Info info);

            Framebuffer(Framebuffer&) = delete;

            Framebuffer(Framebuffer&&);
            void operator=(Framebuffer&&);

            ~Framebuffer();
        private:
            void createFramebuffer(Info& info);

            VkFramebuffer framebuffer;

            Context* context;
    };
}