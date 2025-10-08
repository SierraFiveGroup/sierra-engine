#include "mem_manager.hpp"

namespace Sierra::vlk {
    MemoryManager::MemoryManager(Context& context): context(&context), asyncDat(std::make_shared<AsyncDat>()),
     task(Task::Stage::UPLOAD, 0, &MemoryManager::asyncTransfer, std::reinterpret_pointer_cast<uint8_t>(asyncDat)) {
        asyncDatMutex = std::make_shared<std::mutex>();

        cmdPool = CommandPool(context, context.device->getQueueFamilyIndex(VK_QUEUE_TRANSFER_BIT), VK_COMMAND_POOL_CREATE_TRANSIENT_BIT);
        cmdBuf = CommandBuffer(cmdPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

        asyncDat->cmdBuf = &cmdBuf;
        asyncDat->context = &context;
        asyncDat->mutex = asyncDatMutex;
        asyncDat->transferOps = &transferOps;
    }

    void MemoryManager::addTransferOp(TransferOp op) {
        std::lock_guard lock(*asyncDatMutex);
        transferOps.push_back(op);
    }

    void MemoryManager::asyncTransfer(std::shared_ptr<uint8_t> asyncDat) {
        AsyncDat& dat = *(AsyncDat*)asyncDat.get();

        dat.cmdBuf->begin(nullptr);

        
        dat.mutex->lock();
        for (TransferOp& op : *dat.transferOps) {
            if(op.buffer) { // copy to buff
                VkBufferCopy copyRegion{};
                copyRegion.srcOffset = op.srcOffset;
                copyRegion.dstOffset = op.dstOffset;
                copyRegion.size = op.size;

                vkCmdCopyBuffer(dat.cmdBuf->getCommandBuffer(), op.src, op.buffer, 1, &copyRegion);
                continue;
            }
            //copy to image   

            VkBufferImageCopy copyRegion{};
            copyRegion.bufferOffset = op.srcOffset;
            copyRegion.bufferRowLength = 0;
            copyRegion.bufferImageHeight = 0;
            copyRegion.imageSubresource = op.subResLayers;
            copyRegion.imageOffset = {0};
            copyRegion.imageExtent = op.imageExtent;

            vkCmdCopyBufferToImage(dat.cmdBuf->getCommandBuffer(), op.src, op.image, op.imageLayout, 1, &copyRegion);
        }
        dat.mutex->unlock();

        dat.cmdBuf->end();

        VkCommandBuffer cmdBufHandle = dat.cmdBuf->getCommandBuffer();

        Fence fence = Fence(*dat.context);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 0;
        submitInfo.signalSemaphoreCount = 0;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &cmdBufHandle;

        VK_ERR(vkQueueSubmit(dat.context->device->getQueue(VK_QUEUE_TRANSFER_BIT), 1, &submitInfo, fence.getFence()));

        VkFence fenceHandle = fence.getFence();

        vkWaitForFences(dat.context->device->getDevice(), 1, &fenceHandle, VK_TRUE, (uint64_t)-1);

        for (TransferOp& op : *dat.transferOps) {
            op.callback(op);
        }
    }

    Task MemoryManager::getTask() {
        return task;
    }


    MemoryManager::MemoryManager(MemoryManager&& other) {
        std::lock_guard lock(*asyncDatMutex);

        transferOps = std::move(other.transferOps);
        cmdPool = std::move(other.cmdPool);
        cmdBuf = std::move(other.cmdBuf);
        task = other.task;
        asyncDat = other.asyncDat;
        asyncDatMutex = other.asyncDatMutex;

        asyncDat->cmdBuf = &cmdBuf;

        other.task = Task();
        other.asyncDat.reset();
    }

    void MemoryManager::operator=(MemoryManager&& other) {
        std::lock_guard lock(*asyncDatMutex);

        transferOps = std::move(other.transferOps);
        cmdPool = std::move(other.cmdPool);
        cmdBuf = std::move(other.cmdBuf);
        task = other.task;
        asyncDat = other.asyncDat;
        asyncDatMutex = other.asyncDatMutex;

        asyncDat->cmdBuf = &cmdBuf;

        other.task = Task();
        other.asyncDat.reset();
    }
}