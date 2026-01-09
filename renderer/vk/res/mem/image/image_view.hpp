#pragma once

#include "core/context.hpp"

namespace Sierra::vlk {
    class ImageView {
        public:
            struct Info {
                VkImage image;
                VkImageSubresourceRange range;
                VkFormat format;
            };

            ImageView();
            ImageView(Context& context, Info info);

            ImageView(ImageView&) = delete;

            ImageView(ImageView&&);
            void operator=(ImageView&&);
            
            ~ImageView();

            VkImageView getView();
        private:
            void createView(Info& info);

            VkImageView view;

            Context* context;
    };
}