#include "mem_loader.hpp"

namespace Sierra::vlk {

    MemLoader::MemLoader(): context(nullptr), stagingBuffers(), transferPromises() {

    }

    MemLoader::MemLoader(Context& context, MemoryManager& memManager): context(&context), memManager(&memManager), stagingBuffers(), transferPromises() {

    }

    std::future<std::pair<Buffer, MemLoader::BuffOpt_t>> MemLoader::createBuff(Buffer::Type type, Buffer::Usage usage, uint8_t* dat, size_t size) {// TODO accept buffer info
        if (type == Buffer::Type::HOST_LOCAL || !context->device->isDedicated()) { // we check if we have a discrete gpu cause if we dont, never any point in copying host to host
            std::promise<std::pair<Buffer, BuffOpt_t>> buffPair;

            buffPair.set_value(std::make_pair(createBuffHost(usage, dat, size), std::nullopt));

            return buffPair.get_future(); // in case we wanna do anymore batching later on we still return future here
        } else if (type == Buffer::Type::DEVICE_LOCAL) {
            return createBuffDevice(*memManager, usage, dat, size);
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

    std::future<std::pair<Buffer, MemLoader::BuffOpt_t>> MemLoader::createBuffDevice(MemoryManager& manager, Buffer::Usage usage, uint8_t* dat, size_t size) {
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

        manager.addTransferOp(op);

        OpRes res{};
        res.stagingBuff = std::move(stagingBuff);
        res.buffer = std::move(buffer);

        OpPromise promise{};
        promise.bufferPromise = std::promise<std::pair<Buffer, BuffOpt_t>>();

        transferPromises[(size_t)res.stagingBuff.getBuff()] = {std::move(res), std::move(promise)};

        return promise.bufferPromise.value().get_future();
    }
    
    std::future<std::pair<Buffer, MemLoader::BuffOpt_t>> MemLoader::updateBuff(Buffer&& dst, Buffer& src, uint8_t* dat, size_t size) {
        MemoryManager::TransferOp op{};

        op.src = src.getBuff();
        op.buffer = dst.getBuff();
        op.size = size;
        op.callback = &MemLoader::loadOpCallback;

        OpRes res{};
        res.buffer = std::move(dst);
        res.stagingBuff = std::move(src);

        OpPromise promise{};
        promise.bufferPromise = std::optional<std::promise<std::pair<Buffer, BuffOpt_t>>>();

        transferPromises[(size_t)res.stagingBuff.getBuff()] = {std::move(res), std::move(promise)};

    }

    std::future<std::pair<Image, MemLoader::BuffOpt_t>> MemLoader::createImage(ImageInfo info, uint8_t* dat, size_t size) {
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

        Image image = Image(*context, imageInfo);

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

        memManager->addTransferOp(op);

        OpRes res{};
        res.stagingBuff = std::move(stagingBuff);
        res.image = std::move(image);

        OpPromise promise{};
        promise.imagePromise = std::promise<std::pair<Image, BuffOpt_t>>();

        transferPromises[(size_t)res.stagingBuff.getBuff()] = {std::move(res), std::move(promise)};

        return promise.imagePromise.value().get_future();
    }

    void MemLoader::loadOpCallback(MemoryManager::TransferOp op) {
        MemLoader* loader = *(MemLoader**)op.userDat;

        auto it = loader->transferPromises.find((size_t)op.src);
        auto& pair = it->second;

        OpRes& res = pair.first;
        OpPromise& promise = pair.second;

        //pretty fucked up but tldr, first we do the actual res then we wrap the staging buff in optional
        if (res.buffer.has_value()) {
            promise.bufferPromise.value().set_value({std::move(res.buffer.value()), {std::move(res.stagingBuff)}});
        } else {
            promise.imagePromise.value().set_value({std::move(res.image.value()), {std::move(res.stagingBuff)}});
        }

        loader->transferPromises.erase(it);

        //TODO signal to buffer/image transfer op is complete, or better implement a whole state machine for that
        //TODO erase staging buffers or reuse them idk
    }
}