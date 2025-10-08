#include "mem_loader.hpp"

namespace Sierra::vlk {

    MemLoader::MemLoader(): context(nullptr), selfPtr(std::make_shared<MemLoader*>(this)), stagingBuffers(), bufferPromises() {

    }

    MemLoader::MemLoader(Context& context): context(&context), selfPtr(std::make_shared<MemLoader*>(this)), stagingBuffers(), bufferPromises() {

    }

    std::future<Buffer> MemLoader::memToBuff(MemoryManager* manager, Buffer::Type type, Buffer::Usage usage, uint8_t* dat, size_t size) {
        if (type == Buffer::Type::HOST_LOCAL || !context->device->isDedicated()) { // we check if we have a discrete gpu cause if we dont, never any point in copying host to host
            std::promise<Buffer> buff;

            buff.set_value(memToBuffHost(usage, dat, size));

            return buff.get_future(); // in case we wanna do anymore batching later on we still return future here
        } else if (type == Buffer::Type::DEVICE_LOCAL) {
            return memToBuffDevice(manager, usage, dat, size);
        }

        throw new std::runtime_error("UNREACHABLE");
    }

    Buffer MemLoader::memToBuffHost(Buffer::Usage usage, uint8_t* dat, size_t size) {
        Buffer::Info buffInfo{};
        buffInfo.queueFamilyIndices =
         {context->device->getQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT), context->device->getQueueFamilyIndex(VK_QUEUE_TRANSFER_BIT)};
        buffInfo.size = size;
        buffInfo.type = Buffer::Type::HOST_LOCAL;
        buffInfo.usage = usage;

        Buffer buff = Buffer(*context, buffInfo);
        buff.copyToBuff(dat, size);

        return buff;
    }

    std::future<Buffer> MemLoader::memToBuffDevice(MemoryManager* manager, Buffer::Usage usage, uint8_t* dat, size_t size) {
        if (!manager) throw new std::runtime_error("Memory manager is null when trying invoke it");

        Buffer::Info buffInfo{};
        buffInfo.queueFamilyIndices = {context->device->getQueueFamilyIndex(VK_QUEUE_TRANSFER_BIT)};
        buffInfo.additionalUsageFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        buffInfo.size = size;
        buffInfo.usage = Buffer::Usage::NONE;
        buffInfo.type = Buffer::Type::HOST_LOCAL;

        Buffer stagingBuff = Buffer(*context, buffInfo);
        stagingBuff.copyToBuff(dat, size);

        buffInfo.usage = usage;
        buffInfo.additionalUsageFlags = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        buffInfo.type = Buffer::Type::DEVICE_LOCAL;

        Buffer buffer = Buffer(*context, buffInfo);

        MemoryManager::TransferOp op{};
        
        op.src = stagingBuff.getBuff();
        op.buffer = buffer.getBuff();
        op.size = size;
        op.callback = &MemLoader::loadOpCallback;
        op.userDat = (void*)selfPtr.get();

        manager->addTransferOp(op);

        std::shared_ptr<std::promise<Buffer>> promise = std::make_shared<std::promise<Buffer>>();

        bufferPromises[buffer.getBuff()] = {{std::move(stagingBuff), std::move(buffer) }, promise};

        return promise->get_future();
    }

    void MemLoader::loadOpCallback(MemoryManager::TransferOp op) {
        MemLoader* loader = *(MemLoader**)op.userDat;

        auto it = loader->bufferPromises.find(op.buffer);
        PromisePair_t& pair = it->second;

        pair.second->set_value(std::move(pair.first.deviceBuff));

        loader->bufferPromises.erase(it);

        LOG("TRANSFER OP COMPLETE");

        //TODO signal to buffer/image transfer op is complete, or better implement a whole state machine for that
        //TODO erase staging buffers or reuse them idk
    }
}