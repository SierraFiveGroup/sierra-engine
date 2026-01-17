#include "instance.hpp"

#ifdef DEBUG

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation",
//    "VK_LAYER_LUNARG_api_dump"
};

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {

    if (messageSeverity < VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        return VK_FALSE;

    if(messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        ERROR(pCallbackData->pMessage << "\nTRACE: \n" << std::stacktrace::current());
    else if(messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        WARN(pCallbackData->pMessage << "\nTRACE: \n" << std::stacktrace::current());
    else if(messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
        LOG(pCallbackData->pMessage << "\nTRACE: \n" << std::stacktrace::current());

    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        throw std::runtime_error("Severe validation error, aborting");

    return VK_FALSE;
}

#endif

const std::vector<const char*> extensions = {
#ifdef DEBUG
    "VK_EXT_debug_utils",
#endif
#ifdef __APPLE__
    VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
#endif
};

#ifdef __APPLE__
#define VK_USE_PLATFORM_MACOS_MVK
#endif

namespace Sierra::vlk {
    Instance::Instance() {
        createInstace();
#ifdef DEBUG
        createDebugMessenger();
#endif
    }

    void Instance::createInstace() {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.apiVersion = VK_MAKE_API_VERSION(1, 4, 0, 0);
        appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
        appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 0);
        appInfo.pApplicationName = "test app"; // todo make customizable
        appInfo.pEngineName = "Sierra engine";

        VkInstanceCreateInfo instanceInfo{};
        instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceInfo.pApplicationInfo = &appInfo;
        #ifdef __APPLE__
        instanceInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
        #endif

#ifdef DEBUG
        if (checkLayerSupport()) {
            instanceInfo.ppEnabledLayerNames = validationLayers.data();
            instanceInfo.enabledLayerCount = validationLayers.size();
        }
#endif

        std::vector<const char*> requiredExtensions = getExtentions();

        requiredExtensions.insert(requiredExtensions.end(), extensions.begin(), extensions.end());

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

    void Instance::createDebugMessenger() {
        VkDebugUtilsMessengerCreateInfoEXT messengerInfo{};
        messengerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        messengerInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        messengerInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        messengerInfo.pfnUserCallback = debugCallback;

        auto createDebugUtilsMessenger = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vkInstance, "vkCreateDebugUtilsMessengerEXT");

        VK_ERR(createDebugUtilsMessenger(vkInstance, &messengerInfo, nullptr, &dbgMessenger));
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

#ifdef DEBUG
    void Instance::destroyDebugMessenger() {
        auto destroyDebugUtilsMessenger = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vkInstance, "vkDestroyDebugUtilsMessengerEXT");

        destroyDebugUtilsMessenger(vkInstance, dbgMessenger, nullptr);
    }
#endif

    Instance::~Instance() {
#ifdef DEBUG
        destroyDebugMessenger();
#endif

        vkDestroyInstance(vkInstance, nullptr);
    }

}