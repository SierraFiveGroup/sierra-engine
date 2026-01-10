#include "mem_manager.hpp"

namespace Sierra::vlk {
    MemoryManager::MemoryManager(Context& context): context(&context),
     transferDat(std::make_shared<AsyncTransferDat>()), transitionDat(std::make_shared<AsyncTransitionDat>()),
     transferTask(Task::Stage::UPLOAD, 0, &MemoryManager::asyncTransfer, std::reinterpret_pointer_cast<uint8_t>(transferDat)),
     transitionTask(Task::Stage::INIT, 0, &MemoryManager::asyncTransition, std::reinterpret_pointer_cast<uint8_t>(transitionDat)) {
        transferDatMutex = std::make_shared<std::mutex>();
        transitionDatMutex = std::make_shared<std::mutex>();

        cmdPool = CommandPool(context, context.device->getQueueFamilyIndex(VK_QUEUE_TRANSFER_BIT), VK_COMMAND_POOL_CREATE_TRANSIENT_BIT);
        transferCmdBuf = CommandBuffer(cmdPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
        transitionCmdBuf = CommandBuffer(cmdPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

        transferDat->cmdBuf = &transferCmdBuf;
        transferDat->context = &context;
        transferDat->mutex = transferDatMutex;
        transferDat->transferOps = &transferOps;

        transitionDat->context = &context;
        transitionDat->mutex = transitionDatMutex;
        transitionDat->cmdBuf = &transitionCmdBuf;
        transitionDat->transitionOps = &transitionOps;
    }

    void MemoryManager::addTransferOp(TransferOp op) {
        std::lock_guard lock(*transferDatMutex);
        transferOps.push_back(op);
    }

    void MemoryManager::addTransitionLayoutOp(TransitionLayoutOp op) {
        std::lock_guard lock(*transitionDatMutex);
        transitionOps.push_back(op);
    }

    void MemoryManager::asyncTransition(std::shared_ptr<uint8_t> asyncDat) {
        AsyncTransitionDat& dat = *(AsyncTransitionDat*)asyncDat.get();

        std::vector<VkImageMemoryBarrier> imageBarriers;
        imageBarriers.reserve(dat.transitionOps->size());

        dat.mutex->lock();
        for(TransitionLayoutOp& op : *dat.transitionOps) {
            VkImageMemoryBarrier barrier{};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.srcAccessMask = VK_ACCESS_NONE;
            barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;
            barrier.oldLayout = op.oldLayout;
            barrier.newLayout = op.newLayout;
            barrier.image = op.image;

            barrier.subresourceRange = op.range;
            
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;


            imageBarriers.push_back(barrier);
        }
        dat.mutex->unlock();

        dat.cmdBuf->begin(nullptr);
        vkCmdPipelineBarrier(dat.cmdBuf->getCommandBuffer(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, imageBarriers.size(), imageBarriers.data());
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

        for (TransitionLayoutOp& op : *dat.transitionOps) {
            if(op.callback)
                op.callback(op);
        }
    }

    void MemoryManager::asyncTransfer(std::shared_ptr<uint8_t> asyncDat) {
        AsyncTransferDat& dat = *(AsyncTransferDat*)asyncDat.get();

        dat.cmdBuf->begin(nullptr);

        std::vector<VkBufferMemoryBarrier> buffBarriers{};
        buffBarriers.reserve(dat.transferOps->size());

        std::vector<VkImageMemoryBarrier> imageBarriers{};
        imageBarriers.reserve(dat.transferOps->size());
        
        dat.mutex->lock();
        for (TransferOp& op : *dat.transferOps) {
            if(op.buffer) { // copy to buff
                VkBufferCopy copyRegion{};
                copyRegion.srcOffset = op.srcOffset;
                copyRegion.dstOffset = op.dstOffset;
                copyRegion.size = op.size;

                vkCmdCopyBuffer(dat.cmdBuf->getCommandBuffer(), op.src, op.buffer, 1, &copyRegion);
                VkBufferMemoryBarrier barrier{};
                barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
                barrier.buffer = op.buffer;
                barrier.dstAccessMask = VK_ACCESS_NONE;
                barrier.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
                barrier.offset = 0;
                barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                barrier.size = VK_WHOLE_SIZE;

                buffBarriers.push_back(barrier);

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


            VkImageSubresourceRange subresRange{};
            subresRange.aspectMask = op.subResLayers.aspectMask;
            subresRange.baseArrayLayer = 0;
            subresRange.baseMipLevel = 0;
            subresRange.layerCount = 1;
            subresRange.levelCount = 1;


            VkImageMemoryBarrier barrier{};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.image = op.image;
            barrier.dstAccessMask = VK_ACCESS_NONE;
            barrier.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.subresourceRange = subresRange;
            barrier.oldLayout = op.imageLayout;
            barrier.newLayout = op.imageLayout;

            imageBarriers.push_back(barrier);
        }
        dat.mutex->unlock();

        vkCmdPipelineBarrier(dat.cmdBuf->getCommandBuffer(),
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
            0, 0, nullptr, buffBarriers.size(), buffBarriers.data(),
            imageBarriers.size(), imageBarriers.data());

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
            if(op.callback) 
                op.callback(op);
        }
    }

    std::vector<Task> MemoryManager::getTasks() {
        return {transferTask, transitionTask};
    }


    MemoryManager::MemoryManager(MemoryManager&& other) {
        std::lock_guard lock1(*transferDatMutex);
        std::lock_guard lock2(*transitionDatMutex);

        transferOps = std::move(other.transferOps);
        transitionOps = std::move(other.transitionOps);
        cmdPool = std::move(other.cmdPool);
        transferCmdBuf = std::move(other.transferCmdBuf);
        transitionCmdBuf = std::move(other.transitionCmdBuf); 
        transferTask = other.transferTask;
        transitionTask = other.transitionTask;
        transferDat = other.transferDat;
        transitionDat = other.transitionDat;
        transferDatMutex = other.transferDatMutex;
        transitionDatMutex = other.transitionDatMutex;

        transferDat->cmdBuf = &transferCmdBuf;

        other.transferTask = Task();
        other.transferDat.reset();
        other.transitionTask = Task();
        other.transitionDat.reset();
    }

    void MemoryManager::operator=(MemoryManager&& other) {
        std::lock_guard lock1(*transferDatMutex);
        std::lock_guard lock2(*transitionDatMutex);

        transferOps = std::move(other.transferOps);
        transitionOps = std::move(other.transitionOps);
        cmdPool = std::move(other.cmdPool);
        transferCmdBuf = std::move(other.transferCmdBuf);
        transitionCmdBuf = std::move(other.transitionCmdBuf); 
        transferTask = other.transferTask;
        transitionTask = other.transitionTask;
        transferDat = other.transferDat;
        transitionDat = other.transitionDat;
        transferDatMutex = other.transferDatMutex;
        transitionDatMutex = other.transitionDatMutex;

        transferDat->cmdBuf = &transferCmdBuf;

        other.transferTask = Task();
        other.transferDat.reset();
        other.transitionTask = Task();
        other.transitionDat.reset();
    }
}