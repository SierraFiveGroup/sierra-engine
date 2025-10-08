#include "image.hpp"

namespace Sierra::vlk {

    Image::ImageMap_t Image::imagePtrMap = {}; 

    Image::Image(): image(), mem(), available() {

    }

    Image::Image(Context& context, MemoryManager& manager, Info info): image(), mem(), 
     available(std::make_shared<std::atomic_bool>(true)) {
        createImage(context, info);

        imagePtrMap[image] = std::make_shared<Image*>(this);

        if (info.layout != VK_IMAGE_LAYOUT_UNDEFINED) 
            addTransitionOp(manager, info);
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

    void Image::addTransitionOp(MemoryManager& manager, Info& info) {
        MemoryManager::TransitionLayoutOp op{};

        op.image = image;
        op.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        op.newLayout = info.layout;
        op.callback = &Image::transitionOpCallback;

        manager.addTransitionLayoutOp(op);

        *available = false;
    }

    void Image::transitionOpCallback(MemoryManager::TransitionLayoutOp op) {
        Image& image = **imagePtrMap[op.image];

        image.completeTransition(); 
    }

    void Image::completeTransition() {
        DBG("SKIBID DZABR"); //remove
        *available = true;
    }

    VkImage Image::getImage() {
        return image;
    }

    Image::Image(Image&& other) {
        image = other.image;
        available = other.available;
        mem = std::move(other.mem);

        other.image = VK_NULL_HANDLE;

        *imagePtrMap[image] = this;
    }

    void Image::operator=(Image&& other) {
        image = other.image;
        available = other.available;
        mem = std::move(other.mem);

        other.image = VK_NULL_HANDLE;

        *imagePtrMap[image] = this;
    }

    Image::~Image() {
        if (image)
            imagePtrMap.erase(image);
    }
}