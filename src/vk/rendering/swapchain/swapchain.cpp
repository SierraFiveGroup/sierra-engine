#include "swapchain.hpp"

namespace Sierra::vlk {

    Swapchain::Swapchain() {

    }

    Swapchain::Swapchain(Context& context, Window& window): vkSwapchain(VK_NULL_HANDLE), context(&context) {
        createWindowSurface(window);
        createSwapchain();
        createImageViews(window);
    }

    void Swapchain::createSwapchain() {
        surfaceFormat = selectImageFormat();

        VkSurfaceCapabilitiesKHR capabilites;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(context->device->getPhysicalDevice(), surface, &capabilites);

        uint32_t imageCount;
        if(capabilites.maxImageCount) imageCount = std::clamp(PREFERRED_IMAGE_COUNT, capabilites.minImageCount, capabilites.maxImageCount);
        else imageCount = std::max(PREFERRED_IMAGE_COUNT, capabilites.minImageCount);

        VkSwapchainCreateInfoKHR swapchainInfo{};

        swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainInfo.oldSwapchain = vkSwapchain;
        swapchainInfo.surface = surface; 
        swapchainInfo.imageFormat = surfaceFormat.format;
        swapchainInfo.imageColorSpace = surfaceFormat.colorSpace;
        swapchainInfo.clipped = VK_TRUE;
        swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainInfo.imageArrayLayers = 1;
        swapchainInfo.imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // TODO check
        swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; //DITTO
        swapchainInfo.minImageCount = imageCount;
        swapchainInfo.imageExtent = capabilites.currentExtent;
        swapchainInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        swapchainInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;

        VK_ERR(vkCreateSwapchainKHR(context->device->getDevice(), &swapchainInfo, nullptr, &vkSwapchain));
    }

    void Swapchain::createWindowSurface(Window& window) {
        VK_ERR(glfwCreateWindowSurface(context->instance->getInstance(), window.getGLFWWindow(), nullptr, &surface));
    }

    VkSurfaceFormatKHR Swapchain::selectImageFormat() {
        uint32_t surfaceFormatCount;
        VK_ERR(vkGetPhysicalDeviceSurfaceFormatsKHR(context->device->getPhysicalDevice(), surface, &surfaceFormatCount, nullptr));

        std::vector<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatCount);
        VK_ERR(vkGetPhysicalDeviceSurfaceFormatsKHR(context->device->getPhysicalDevice(), surface, &surfaceFormatCount, surfaceFormats.data()));

        VK_ASSERT(!surfaceFormats.empty());

        return surfaceFormats.front();
    }

    void Swapchain::createImageViews(Window& window) {
        uint32_t imageCount;
        VK_ERR(vkGetSwapchainImagesKHR(context->device->getDevice(), vkSwapchain, &imageCount, nullptr));

        std::vector<VkImage> images(imageCount);
        VK_ERR(vkGetSwapchainImagesKHR(context->device->getDevice(), vkSwapchain, &imageCount, images.data()));

        VkImageSubresourceRange subresRange{};
        subresRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        subresRange.baseMipLevel = 0;
        subresRange.baseArrayLayer = 0;
        subresRange.levelCount = 1;
        subresRange.layerCount = 1;

        ImageView::Info viewInfo{};
        viewInfo.format = surfaceFormat.format;
        viewInfo.range = subresRange;

        imageViews.reserve(imageCount);
        imageViewHandles.reserve(imageCount);
        for(VkImage image : images) {
            viewInfo.image = image;

            imageViews.emplace_back(
                *context, viewInfo
            );

            imageViewHandles.push_back(
                imageViews.back().getView()
            );
        }
    }

    VkSurfaceFormatKHR Swapchain::getSurfaceFormat() {
        return surfaceFormat;
    }

    std::vector<VkImageView> Swapchain::getImageViews() {
        return imageViewHandles; 
    }

    Swapchain::Swapchain(Swapchain&& other): context(other.context) {
        vkSwapchain = other.vkSwapchain;
        context = other.context;
        surface = other.surface;
        surfaceFormat = other.surfaceFormat;
        imageViews = std::move(other.imageViews);
        imageViewHandles = other.imageViewHandles;

        other.vkSwapchain = VK_NULL_HANDLE;
        other.surface = VK_NULL_HANDLE;
        other.context = VK_NULL_HANDLE;
        other.imageViewHandles = {};
    }

    void Swapchain::operator=(Swapchain&& other) {
        vkSwapchain = other.vkSwapchain;
        context = other.context;
        surface = other.surface;
        surfaceFormat = other.surfaceFormat;
        imageViews = std::move(other.imageViews);
        imageViewHandles = other.imageViewHandles;

        other.vkSwapchain = VK_NULL_HANDLE;
        other.surface = VK_NULL_HANDLE;
        other.context = VK_NULL_HANDLE;
        other.imageViewHandles = {};
    }

    Swapchain::~Swapchain() {
        if(vkSwapchain)
            vkDestroySwapchainKHR(context->device->getDevice(), vkSwapchain, nullptr);
        if(surface)
            vkDestroySurfaceKHR(context->instance->getInstance(), surface, nullptr);
    }
}