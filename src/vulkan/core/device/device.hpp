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

            ~Device();
        private:

            void pickPhysicalDevice(Instance* instance);
            std::vector<VkDeviceQueueCreateInfo> getQueueInfos();
            void createLogicalDevice();
            void retrieveQueues();

            std::vector<VkQueue> computeQueues;
            std::vector<VkQueue> graphicsQueues;
            std::vector<VkQueue> transferQueues;

            VkPhysicalDevice physicalDevice;
            VkDevice vkDevice;
    };
}