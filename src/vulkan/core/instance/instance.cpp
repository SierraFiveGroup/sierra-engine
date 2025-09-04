#include "instance.hpp"

#ifdef DEBUG

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {

    if (messageSeverity < VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        return VK_FALSE;

    if(messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        ERROR(pCallbackData->pMessage);
    else if(messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        WARN(pCallbackData->pMessage);
    else if(messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
        LOG(pCallbackData->pMessage);

    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        throw new std::runtime_error("Severe validation error, aborting");

    return VK_FALSE;
}

#endif


const std::vector<const char*> extensions = {};

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
        instanceInfo.pApplicationInfo = &appInfo;

#ifdef DEBUG
        if (checkLayerSupport()) {
            instanceInfo.ppEnabledLayerNames = validationLayers.data();
            instanceInfo.enabledLayerCount = validationLayers.size();
        }
#endif

        std::vector<const char*> requiredExtensions = getExtentions();

        instanceInfo.enabledExtensionCount = requiredExtensions.size();
        instanceInfo.ppEnabledExtensionNames = requiredExtensions.data();

        VK_ERR(vkCreateInstance(&instanceInfo, nullptr, &vkInstance));
    }

#ifdef DEBUG
    bool Instance::checkLayerSupport() {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data()); 

        for(const char* layer : validationLayers) {
            bool found = false;
            for(VkLayerProperties property : availableLayers) {
                if(strcmp(property.layerName, layer)) {
                    found = true;
                    break;
                }
            }

            if (!found){
                ERROR("Not all required validation layers found, Vulkan debugging disabled");
                return false;
            }
        }

        return true;
    }
#endif

    std::vector<const char*> Instance::getExtentions() {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> combinedExtensions(glfwExtensionCount + extensions.size());

        memcpy(combinedExtensions.data(), glfwExtensions, glfwExtensionCount * sizeof(size_t));
        std::copy(extensions.begin(), extensions.end(), combinedExtensions.begin() + glfwExtensionCount);

        return combinedExtensions;
    }

    VkInstance Instance::getInstance() { 
        return vkInstance;
    }

    Instance::~Instance() {
        vkDestroyInstance(vkInstance, nullptr);
    }

}