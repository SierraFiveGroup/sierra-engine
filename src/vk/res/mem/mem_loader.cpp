#include "mem_loader.hpp"

namespace Sierra::vlk {

    MemLoader::MemLoader(): context(nullptr), selfPtr(std::make_shared<MemLoader*>(this)), stagingBuffers(), bufferPromises() {

    }

    MemLoader::MemLoader(Context& context): context(&context), selfPtr(std::make_shared<MemLoader*>(this)), stagingBuffers(), bufferPromises() {

    }

    std::future<Buffer> MemLoader::createBuff(MemoryManager& manager, Buffer::Type type, Buffer::Usage usage, uint8_t* dat, size_t size) {
        if (type == Buffer::Type::HOST_LOCAL || !context->device->isDedicated()) { // we check if we have a discrete gpu cause if we dont, never any point in copying host to host
            std::promise<Buffer> buff;

            buff.set_value(createBuffHost(usage, dat, size));

            return buff.get_future(); // in case we wanna do anymore batching later on we still return future here
        } else if (type == Buffer::Type::DEVICE_LOCAL) {
            return createBuffDevice(manager, usage, dat, size);
        }

        throw new std::runtime_error("UNREACHABLE");
    }

    Buffer MemLoader::createBuffHost(Buffer::Usage usage, uint8_t* dat, size_t size) {
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

    std::future<Buffer> MemLoader::createBuffDevice(MemoryManager& manager, Buffer::Usage usage, uint8_t* dat, size_t size) {
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

        manager.addTransferOp(op);

        std::shared_ptr<std::promise<Buffer>> promise = std::make_shared<std::promise<Buffer>>();

        bufferPromises[buffer.getBuff()] = {{std::move(stagingBuff), std::move(buffer) }, promise};

        return promise->get_future();
    }

    std::future<Image> MemLoader::createImage(MemoryManager& manager, ImageInfo info, uint8_t* dat, size_t size) {
        std::vector<uint32_t> queueFamilyIndices = 
         {context->device->getQueueFamilyIndex(VK_QUEUE_TRANSFER_BIT), context->device->getQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT)};

        Image::Info imageInfo{};
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.format = info.format;
        imageInfo.extent = info.extent;
        imageInfo.mipLevels = 1;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.queueFamilyIndices = queueFamilyIndices;
        imageInfo.layout = info.layout;
        imageInfo.usage = info.usage;
        imageInfo.type = Mem::Type::PREFER_DEVICE;

        Image image = Image(*context, manager, imageInfo);

        Buffer::Info buffInfo{};
        buffInfo.queueFamilyIndices = {context->device->getQueueFamilyIndex(VK_QUEUE_TRANSFER_BIT)};
        buffInfo.additionalUsageFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        buffInfo.size = size;
        buffInfo.usage = Buffer::Usage::NONE;
        buffInfo.type = Buffer::Type::HOST_LOCAL;

        Buffer stagingBuff = Buffer(*context, buffInfo);
        stagingBuff.copyToBuff(dat, size);

        VkImageSubresourceLayers layers{};
        if (imageInfo.usage == VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
            layers.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        else if (imageInfo.usage == VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
            layers.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT; // TODO correct?
        else
            throw new std::runtime_error("Image usage transfer not supported");
        layers.layerCount = 1;

        MemoryManager::TransferOp op{};
        op.src = stagingBuff.getBuff();
        op.size = size;

        op.image = image.getImage();
        op.subResLayers = layers;
        op.imageExtent = info.extent;
        op.imageLayout = info.layout;
        op.callback = &MemLoader::loadOpCallback; 
        op.userDat = (void*)selfPtr.get();

        manager.addTransferOp(op);

        std::shared_ptr<std::promise<Image>> promise = std::make_shared<std::promise<Image>>();

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