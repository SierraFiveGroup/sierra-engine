#include "mem_loader.hpp"

namespace Sierra::vlk {

    MemLoader::MemLoader(): context(nullptr), selfPtr(std::make_shared<MemLoader*>(this)), stagingBuffers(), transferPromises() {

    }

    MemLoader::MemLoader(Context& context): context(&context), selfPtr(std::make_shared<MemLoader*>(this)), stagingBuffers(), transferPromises() {

    }

    std::future<Buffer> MemLoader::createBuff(MemoryManager& manager, Buffer::Type type, Buffer::Usage usage, uint8_t* dat, size_t size) {// TODO accept buffer info
        if (type == Buffer::Type::HOST_LOCAL || !context->device->isDedicated()) { // we check if we have a discrete gpu cause if we dont, never any point in copying host to host
            std::promise<Buffer> buff;

            buff.set_value(createBuffHost(usage, dat, size));

            return buff.get_future(); // in case we wanna do anymore batching later on we still return future here
        } else if (type == Buffer::Type::DEVICE_LOCAL) {
            return createBuffDevice(manager, usage, dat, size);
        }

        throw std::runtime_error("UNREACHABLE");
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

        OpRes res{};
        res.stagingBuff = std::move(stagingBuff);
        res.buffer = std::move(buffer);

        OpPromise promise{};
        promise.bufferPromise = std::make_shared<std::promise<Buffer>>();

        transferPromises[(size_t)res.stagingBuff.getBuff()] =
         {std::move(res), promise};

        return promise.bufferPromise.value()->get_future();
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
        imageInfo.usage = info.usage | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
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
        if (imageInfo.usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
            layers.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT; // TODO correct?
        else
            layers.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
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

        OpRes res{};
        res.stagingBuff = std::move(stagingBuff);
        res.image = std::move(image);

        OpPromise promise{};
        promise.imagePromise = std::make_shared<std::promise<Image>>();

        transferPromises[(size_t)res.stagingBuff.getBuff()] =
         {std::move(res), promise};

        return promise.imagePromise.value()->get_future();
    }

    void MemLoader::loadOpCallback(MemoryManager::TransferOp op) {
        MemLoader* loader = *(MemLoader**)op.userDat;

        auto it = loader->transferPromises.find((size_t)op.src);
        PromisePair_t& pair = it->second;

        OpRes& res = pair.first;
        OpPromise& promise = pair.second;

        if (res.buffer.has_value()) {
            promise.bufferPromise.value()->set_value(std::move(res.buffer.value()));
        } else {
            promise.imagePromise.value()->set_value(std::move(res.image.value()));
        }

        loader->transferPromises.erase(it);

        //TODO signal to buffer/image transfer op is complete, or better implement a whole state machine for that
        //TODO erase staging buffers or reuse them idk
    }
}