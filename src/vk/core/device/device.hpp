#pragma once

#include "../instance/instance.hpp"

#include <vulkan/vulkan.h>

#include <vector>

namespace Sierra::vlk {
    class Device {
        public:
            Device(Instance* instance);

            Device(Device&) = delete;
            Device(Device&&) = delete;

            VkQueue getQueue(VkQueueFlags queueType);
            uint32_t getQueueFamilyIndex(VkQueueFlags queueType);

            VkPhysicalDevice getPhysicalDevice();
            VkDevice getDevice();

            ~Device();
        private:

            void pickPhysicalDevice(Instance* instance);
            std::vector<VkDeviceQueueCreateInfo> getQueueInfos();
            void createLogicalDevice();
            void retrieveQueues();

            std::vector<const char*> getExtensions();

            std::vector<VkQueue> computeQueues;
            std::vector<VkQueue> graphicsQueues;
            std::vector<VkQueue> transferQueues;

            uint32_t computeQueueIndex;
            uint32_t graphicsQueueIndex;
            uint32_t transferQueueIndex;

            uint32_t computeQueueFamilyIndex;
            uint32_t graphicsQueueFamilyIndex;
            uint32_t transferQueueFamilyIndex;

            VkPhysicalDevice physicalDevice;
            VkDevice vkDevice;
    };
}