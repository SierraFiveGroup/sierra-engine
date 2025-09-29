#include "device.hpp"

const std::vector<const char*> requiredExtensions = {
    "VK_KHR_swapchain"
};

namespace Sierra::vlk {

    Device::Device(Instance* instance): computeQueueFamilyIndex(0), transferQueueFamilyIndex(0), graphicsQueueFamilyIndex(0),
     transferQueueIndex(0), graphicsQueueIndex(0), computeQueueIndex(0) {
        pickPhysicalDevice(instance);
        createLogicalDevice();
        retrieveQueues();
    }

    void Device::pickPhysicalDevice(Instance* instance) {
        uint32_t devCount;
        vkEnumeratePhysicalDevices(instance->getInstance(), &devCount, nullptr);
        
        std::vector<VkPhysicalDevice> physicalDevices;
        physicalDevices.resize(devCount);
        vkEnumeratePhysicalDevices(instance->getInstance(), &devCount, physicalDevices.data());

        if (!devCount) throw new std::runtime_error("No physical devices(GPUs) found");
        
        physicalDevice = physicalDevices.front(); // if we dont find a dedicated gpu we'll still have something 
        for(VkPhysicalDevice dev : physicalDevices) {
            VkPhysicalDeviceProperties properties;

            vkGetPhysicalDeviceProperties(dev, &properties);

            if(properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) physicalDevice = dev; 
        }
    }

    std::vector<VkDeviceQueueCreateInfo> Device::getQueueInfos() {
        static float priorities[64] = {1.0f};

        uint32_t propertyCount;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &propertyCount, nullptr);

        std::vector<VkQueueFamilyProperties> properties(propertyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &propertyCount, properties.data());

        std::vector<VkDeviceQueueCreateInfo> queueInfos;

        VkDeviceQueueCreateInfo queueInfo{};
        queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueInfo.pQueuePriorities = priorities;

        

        
        for(int i = 0; i < properties.size(); i++) {
            if(! (properties[i].queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT)) ) {
                continue;
            }

            queueInfo.queueCount = properties[i].queueCount;
            queueInfo.queueFamilyIndex = i;
            queueInfos.push_back(queueInfo);
        }

        return queueInfos;
    }

    void Device::createLogicalDevice() {
        VkPhysicalDeviceFeatures2 features2{};
        features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;

        vkGetPhysicalDeviceFeatures2(physicalDevice, &features2);

        VkPhysicalDeviceDescriptorIndexingFeatures descriptorIndexingFeatures{};
        descriptorIndexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
        descriptorIndexingFeatures.descriptorBindingSampledImageUpdateAfterBind = true;
        descriptorIndexingFeatures.descriptorBindingStorageBufferUpdateAfterBind = true;
        descriptorIndexingFeatures.descriptorBindingUniformBufferUpdateAfterBind = true;
        descriptorIndexingFeatures.descriptorBindingStorageImageUpdateAfterBind = true;
        descriptorIndexingFeatures.descriptorBindingUniformTexelBufferUpdateAfterBind = true;
        descriptorIndexingFeatures.descriptorBindingStorageBufferUpdateAfterBind = true;
        descriptorIndexingFeatures.descriptorBindingStorageTexelBufferUpdateAfterBind = true;
        descriptorIndexingFeatures.descriptorBindingPartiallyBound = true;

        features2.pNext = &descriptorIndexingFeatures;


        const std::vector<const char*> extensions = getExtensions();

        std::vector<VkDeviceQueueCreateInfo> queueInfos = getQueueInfos();

        VkDeviceCreateInfo deviceInfo{};
        deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        deviceInfo.pQueueCreateInfos = queueInfos.data();
        deviceInfo.queueCreateInfoCount = queueInfos.size();

        deviceInfo.enabledExtensionCount = extensions.size();
        deviceInfo.ppEnabledExtensionNames = extensions.data();

        deviceInfo.pNext = &features2;

        deviceInfo.enabledLayerCount = 0;

        VK_ERR(vkCreateDevice(physicalDevice, &deviceInfo, nullptr, &vkDevice));

    }

    void Device::retrieveQueues() {
        uint32_t propertyCount;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &propertyCount, nullptr);

        std::vector<VkQueueFamilyProperties> properties(propertyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &propertyCount, properties.data());

        auto copyToVector = [&](std::vector<VkQueue>& vec, uint32_t familyIndex, uint32_t queueCount) {
            uint32_t vecOffset = vec.size();
            vec.resize(queueCount + vec.size());

            for(int i = 0; i < queueCount; i++) {
                vkGetDeviceQueue(vkDevice, familyIndex, i, &vec[i + vecOffset]);
            }
        };

        for(int i = 0; i < properties.size(); i++) {
            if (properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT && !graphicsQueueFamilyIndex) {
                copyToVector(graphicsQueues, i, properties[i].queueCount);
                graphicsQueueFamilyIndex = i;
            }
            else if (properties[i].queueFlags & VK_QUEUE_TRANSFER_BIT && !transferQueueFamilyIndex) {
                copyToVector(transferQueues, i, properties[i].queueCount);
                transferQueueFamilyIndex = i;
            }
            else if (properties[i].queueFlags & VK_QUEUE_COMPUTE_BIT && !computeQueueFamilyIndex) {
                copyToVector(computeQueues, i, properties[i].queueCount);
                computeQueueFamilyIndex = i;
            }
        }
    }

    VkQueue Device::getQueue(VkQueueFlags queueType) {
        switch(queueType) {
            case VK_QUEUE_GRAPHICS_BIT:
                return graphicsQueues[++graphicsQueueIndex % graphicsQueues.size()];
            case VK_QUEUE_TRANSFER_BIT:
                return transferQueues[++transferQueueIndex % transferQueues.size()];
            case VK_QUEUE_COMPUTE_BIT:
                return computeQueues[++computeQueueIndex % computeQueues.size()];
        }

        return VK_NULL_HANDLE;
    }

    uint32_t Device::getQueueFamilyIndex(VkQueueFlags queueType) {
        switch (queueType) {
            case VK_QUEUE_GRAPHICS_BIT:
                return graphicsQueueFamilyIndex;
            case VK_QUEUE_TRANSFER_BIT:
                return transferQueueFamilyIndex;
            case VK_QUEUE_COMPUTE_BIT:
                return computeQueueFamilyIndex;
        }

        ERROR("Tried to get the queue family index of nonexistent family " << queueType);

        return -1;
    }

    std::vector<const char*> Device::getExtensions() {
        uint32_t deviceExtensionCount;
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtensionCount, nullptr);

        std::vector<VkExtensionProperties> deviceExtensions(deviceExtensionCount);
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtensionCount, deviceExtensions.data());

        for(const char* ext : requiredExtensions) {
            bool found = false;
            for(VkExtensionProperties property : deviceExtensions) {
                if(strcmp(property.extensionName, ext)) {
                    found = true;
                    break;
                }
            }

            if (!found){
                throw new std::runtime_error("Not all required device extensions found");
            }
        }

        return requiredExtensions;
    }

    VkPhysicalDevice Device::getPhysicalDevice() {
        return physicalDevice;
    }

    VkDevice Device::getDevice() {
        return vkDevice;
    }
 
    Device::~Device() {
        vkDestroyDevice(vkDevice, nullptr);
    }
}