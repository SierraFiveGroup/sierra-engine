#include "device.hpp"

namespace Sierra::vk {

    Device::Device(Instance* instance) {
        pickPhysicalDevice(instance);
        createLogicalDevice();
        retrieveQueues();
    }

    void Device::pickPhysicalDevice(Instance* instance) {
        uint32_t devCount;
        vkEnumeratePhysicalDevices(instance->getVkInstance(), &devCount, nullptr);
        
        std::vector<VkPhysicalDevice> physicalDevices;
        physicalDevices.resize(devCount);
        vkEnumeratePhysicalDevices(instance->getVkInstance(), &devCount, physicalDevices.data());

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
        std::vector<VkDeviceQueueCreateInfo> queueInfos = getQueueInfos();

        VkDeviceCreateInfo deviceInfo{};
        deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        deviceInfo.pQueueCreateInfos = queueInfos.data();
        deviceInfo.queueCreateInfoCount = queueInfos.size();

        deviceInfo.enabledExtensionCount = 0;
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
            if (properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) 
                copyToVector(graphicsQueues, i, properties[i].queueCount);
            else if (properties[i].queueFlags & VK_QUEUE_TRANSFER_BIT) 
                copyToVector(transferQueues, i, properties[i].queueCount);
            else if (properties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) 
                copyToVector(computeQueues, i, properties[i].queueCount);
        }
    }

    VkQueue Device::getQueue(VkQueueFlags queueType) {
        if (queueType == VK_QUEUE_GRAPHICS_BIT) return graphicsQueues[++graphicsQueueIndex % graphicsQueues.size()];
        else if(queueType == VK_QUEUE_TRANSFER_BIT) return transferQueues[++transferQueueIndex % transferQueues.size()];
        else if(queueType == VK_QUEUE_COMPUTE_BIT) return computeQueues[++computeQueueIndex % computeQueues.size()];

        return VK_NULL_HANDLE;
    }

    VkPhysicalDevice Device::getPhysicalDevice() {
        return physicalDevice;
    }

    VkDevice Device::getDevice() {
        vkDevice;
    }
 
    Device::~Device() {
        vkDestroyDevice(vkDevice, nullptr);
    }
}