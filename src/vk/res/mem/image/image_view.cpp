#include "image_view.hpp"

namespace Sierra::vlk {
    ImageView::ImageView(): context(), view(){

    }

    ImageView::ImageView(Context& context, Info info): context(&context), view() {
        createView(info);
    }

    void ImageView::createView(Info& info) {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = info.image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = info.format;
        viewInfo.components =
         {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A};
         // TODO SHOULD BE IDENTITY??
        viewInfo.subresourceRange = info.range;

        VK_ERR(vkCreateImageView(context->device->getDevice(), &viewInfo, nullptr, &view));
    }

    VkImageView ImageView::getView() {
        return view;
    }

    ImageView::ImageView(ImageView&& other) {
        view = other.view;
        context = other.context;

        other.view = VK_NULL_HANDLE;
        other.context = nullptr;
    }

    void ImageView::operator=(ImageView&& other) {
        view = other.view;
        context = other.context;

        other.view = VK_NULL_HANDLE;
        other.context = nullptr;
    }
            
    ImageView::~ImageView() {
        if (view)
            vkDestroyImageView(context->device->getDevice(), view, nullptr);
    }
}