#include "command_pool.hpp"

namespace Sierra::vlk {

    CommandPool::CommandPool(): commandPool(), context() {

    }

    CommandPool::CommandPool(Context& context, uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags): commandPool(),
     context(&context), resetCmdBuffer() {
        createPool(context, queueFamilyIndex, flags);

        if(flags | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT)
            resetCmdBuffer = true;
    }

    void CommandPool::createPool(Context& context, uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags) {
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = queueFamilyIndex;
        poolInfo.flags = flags;

        VK_ERR(vkCreateCommandPool(context.device->getDevice(), &poolInfo, nullptr, &commandPool));
    }

    VkCommandBuffer CommandPool::allocCmdBuffer(VkCommandBufferLevel level) {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool;
        allocInfo.level = level;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer buff;
        VK_ERR(vkAllocateCommandBuffers(context->device->getDevice(), &allocInfo, &buff));

        return buff;
    }

    void CommandPool::freeCmdBuffer(VkCommandBuffer buffer) {
        vkFreeCommandBuffers(context->device->getDevice(), commandPool, 1, &buffer);
    }

    bool CommandPool::canResetCmdBuffer() {
        return resetCmdBuffer;
    }

    CommandPool::CommandPool(CommandPool&& other) {
        commandPool = other.commandPool;
        context = other.context;
        resetCmdBuffer = other.resetCmdBuffer;

        other.commandPool = VK_NULL_HANDLE;
        other.context = nullptr;
    }

    void CommandPool::operator=(CommandPool&& other) {
        commandPool = other.commandPool;
        context = other.context;
        resetCmdBuffer = other.resetCmdBuffer;

        other.commandPool = VK_NULL_HANDLE;
        other.context = nullptr;
    }

    CommandPool::~CommandPool() {
        if (commandPool)
            vkDestroyCommandPool(context->device->getDevice(), commandPool, nullptr);
    }

}