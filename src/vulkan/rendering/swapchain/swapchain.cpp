#include "swapchain.hpp"

namespace Sierra::vk {
    Swapchain::Swapchain(Context& context): vkSwapchain(VK_NULL_HANDLE) {

    }

    void Swapchain::createSwapchain(Context& context) {
        VkSwapchainCreateInfoKHR swapchainInfo{};

        swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainInfo.oldSwapchain = vkSwapchain;
        swapchainInfo.surface = nullptr; 
        #error no surfaces?
        swapchainInfo.
    }


    VkFormat Swapchain::selectImageFormat(Context& context) {
        vk
    }
}