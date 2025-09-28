#include "command_buffer.hpp"

namespace Sierra::vlk {
    CommandBuffer::CommandBuffer(): buffer(), level(), pool() {

    }

    CommandBuffer::CommandBuffer(CommandPool& pool, VkCommandBufferLevel level): buffer(), level(level), pool(&pool) {
        buffer = pool.allocCmdBuffer(level);
    }

    void CommandBuffer::begin(VkCommandBufferInheritanceInfo *inheritanceInfo) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.pInheritanceInfo = inheritanceInfo;

        VK_ERR(vkBeginCommandBuffer(buffer, &beginInfo));
    }

    void CommandBuffer::end() {
        VK_ERR(vkEndCommandBuffer(buffer));
    }

    bool CommandBuffer::canReset() {
        return pool->canResetCmdBuffer();
    }

    void CommandBuffer::reset() {
        vkResetCommandBuffer(buffer, 0);
    }

    VkCommandBuffer CommandBuffer::getCommandBuffer() {
        return buffer;
    }
    
    CommandBuffer::CommandBuffer(CommandBuffer&& other) {
        buffer = other.buffer;
        level = other.level;
        
        other.buffer = VK_NULL_HANDLE;
    }

    void CommandBuffer::operator=(CommandBuffer&& other) {
        buffer = other.buffer;
        level = other.level;
        
        other.buffer = VK_NULL_HANDLE;
    }

    CommandBuffer::~CommandBuffer() {
        if(buffer)
            pool->freeCmdBuffer(buffer);
    }
}