#include "image.hpp"

namespace Sierra::vlk {

    Image::ImageMap_t Image::imagePtrMap = {}; 

    Image::Image(): image(), mem(), available(std::make_shared<std::atomic_bool>(false)) {

    }

    Image::Image(Context& context, MemoryManager& manager, Info info): image(), mem(), 
     available(std::make_shared<std::atomic_bool>(true)) {
        createImage(context, info);
        createImageView(context, info);

        imagePtrMap[image] = this;

        if (info.layout != VK_IMAGE_LAYOUT_UNDEFINED) 
            addTransitionOp(manager, info);
    }

    Image::Image(Context& context, Info info): image(), mem(), 
     available(std::make_shared<std::atomic_bool>(true)) {
        createImage(context, info);
        createImageView(context, info);

        imagePtrMap[image] = this;

        if (info.layout != VK_IMAGE_LAYOUT_UNDEFINED) 
            throw std::runtime_error("If no MemoryManager is provided layout must be undefined");
    }

    void Image::createImage(Context& context, Info& info) {
        VK_ASSERT(!info.queueFamilyIndices.empty());

        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = info.imageType;
        imageInfo.format = info.format;
        imageInfo.extent = info.extent;
        imageInfo.mipLevels = info.mipLevels;
        imageInfo.arrayLayers = 1;
        imageInfo.samples = info.samples;
        imageInfo.tiling = info.tiling;
        imageInfo.usage = info.usage;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        imageInfo.queueFamilyIndexCount = info.queueFamilyIndices.size();
        imageInfo.pQueueFamilyIndices = info.queueFamilyIndices.data();
        imageInfo.sharingMode = info.queueFamilyIndices.size() == 1 ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT;

        Mem::Info memInfo{};
        memInfo.type = info.type;

        mem = Mem(context, memInfo, imageInfo, image);
    }

    void Image::createImageView(Context& context, Info& info) {
        VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
        if(info.usage == VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
            aspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT; // TODO implement this properly
            //don't just make an educated guess

        ImageView::Info viewInfo{};
        viewInfo.format = info.format;
        viewInfo.image = image;
        viewInfo.range = {aspectFlags, 0, 1, 0, 1};

        view = ImageView(context, viewInfo);
    }

    void Image::addTransitionOp(MemoryManager& manager, Info& info) {
        MemoryManager::TransitionLayoutOp op{};

        op.image = image;
        op.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        op.newLayout = info.layout;
        op.range = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}; // ik this will bite me in the ass later lmfao
        op.callback = &Image::transitionOpCallback;

        manager.addTransitionLayoutOp(op);

        *available = false;
    }

    void Image::transitionOpCallback(MemoryManager::TransitionLayoutOp op) {
        Image& image = *imagePtrMap[op.image];

        image.completeTransition(); 
    }

    void Image::completeTransition() {
        *available = true;
    }

    VkImage Image::getImage() {
        return image;
    }

    VkImageView Image::getView() {
        return view.getView();
    }

    Image::Image(Image&& other) {

        image = std::move(other.image);
        available = std::move(other.available);
        mem = std::move(other.mem);
        view = std::move(other.view);

        if(view.getView() == VK_NULL_HANDLE) return; // uninitialized, we dgaf, we dont do this before in case its done to reset it

        imagePtrMap[image] = this;
    }

    void Image::operator=(Image&& other) {
        image = std::move(other.image);
        available = std::move(other.available);
        mem = std::move(other.mem);
        view = std::move(other.view);

        if(view.getView() == VK_NULL_HANDLE) return; 

        imagePtrMap[image] = this;
    }

    Image::~Image() {
        if (image && imagePtrMap[image] == this)
            imagePtrMap.erase(image);
    }
}