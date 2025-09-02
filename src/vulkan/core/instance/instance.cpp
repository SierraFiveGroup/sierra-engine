#include "instance.hpp"

namespace Sierra::vk {
    Instance::Instance() {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.apiVersion = VK_MAKE_API_VERSION(1, 3, 0, 0);
        appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
        appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 0);
        appInfo.pApplicationName = "test app"; // todo make customizable
        appInfo.pEngineName = "Sierra engine";

        VkInstanceCreateInfo instanceInfo{};
        instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceInfo.enabledExtensionCount = 0;
        instanceInfo.enabledLayerCount = 0;
        instanceInfo.pApplicationInfo = &appInfo;


        VK_ERR(vkCreateInstance(&instanceInfo, nullptr, &vkInstance));
    }

    VkInstance Instance::getVkInstance() { 
        return vkInstance;
    }

    Instance::~Instance() {
        vkDestroyInstance(vkInstance, nullptr);
    }
}