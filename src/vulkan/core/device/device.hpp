#pragma once

#include "../instance/instance.hpp"

#include <vulkan/vulkan.h>

#include <vector>

namespace Sierra::vk {
    class Device {
        public:
            Device(Instance* instance);

            Device(Device&) = delete;
            Device(Device&&) = delete;

            VkQueue getQueue(VkQueueFlags queueType);

            ~Device();
        private:

            void pickPhysicalDevice(Instance* instance);
            std::vector<VkDeviceQueueCreateInfo> getQueueInfos();
            void createLogicalDevice();
            void retrieveQueues();

            std::vector<VkQueue> computeQueues;
            std::vector<VkQueue> graphicsQueues;
            std::vector<VkQueue> transferQueues;

            uint32_t computeQueueIndex;
            uint32_t graphicsQueueIndex;
            uint32_t transferQueueIndex;

            VkPhysicalDevice physicalDevice;
            VkDevice vkDevice;
    };
}