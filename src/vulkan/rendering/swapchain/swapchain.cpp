#include "swapchain.hpp"

namespace Sierra::vk {

    Swapchain::Swapchain() {

    }

    Swapchain::Swapchain(Context& context, Window& window): vkSwapchain(VK_NULL_HANDLE), context(&context) {
        createWindowSurface(window);
        createSwapchain();
    }

    void Swapchain::createSwapchain() {
        VkSurfaceFormatKHR surfaceFormat = selectImageFormat();

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
        swapchainInfo.imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT; // TODO check
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
        vkGetPhysicalDeviceSurfaceFormatsKHR(context->device->getPhysicalDevice(), surface, &surfaceFormatCount, nullptr);

        std::vector<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(context->device->getPhysicalDevice(), surface, &surfaceFormatCount, surfaceFormats.data());

        VK_ASSERT(!surfaceFormats.empty());

        return surfaceFormats.front();
    }

    Swapchain::Swapchain(Swapchain&& other): context(other.context) {
        vkSwapchain = other.vkSwapchain;
        context = other.context;
        surface = other.surface;

        other.vkSwapchain = VK_NULL_HANDLE;
        other.surface = VK_NULL_HANDLE;
    }

    void Swapchain::operator=(Swapchain&& other) {
        vkSwapchain = other.vkSwapchain;
        context = other.context;
        surface = other.surface;


        other.vkSwapchain = VK_NULL_HANDLE;
        other.surface = VK_NULL_HANDLE;
    }

    Swapchain::~Swapchain() {
        vkDestroySwapchainKHR(context->device->getDevice(), vkSwapchain, nullptr);
        vkDestroySurfaceKHR(context->instance->getInstance(), surface, nullptr);
    }
}