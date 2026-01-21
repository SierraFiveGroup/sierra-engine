#include "buffer.hpp"

namespace Sierra::vlk {
    Buffer::Buffer(): mem(), buff(), type(), descriptorInfo() {

    }

    Buffer::Buffer(Context& context, Info& info): type(info.type),
     descriptorInfo(std::make_shared<VkDescriptorBufferInfo>()) {
        createBuff(context, info);
        initDescriptorInfo();
    }

    void Buffer::initDescriptorInfo() {
        descriptorInfo->buffer = buff;
        descriptorInfo->offset = 0;//mem.getOffset(); // TODO figure out why the offset is fucked
        descriptorInfo->range = mem.getSize();
    }

    void Buffer::createBuff(Context& context, Info& info) {
        VK_ASSERT(!info.queueFamilyIndices.empty())

        VkBufferCreateInfo buffInfo{};
        buffInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffInfo.usage = info.additionalUsageFlags;
        buffInfo.queueFamilyIndexCount = info.queueFamilyIndices.size();
        buffInfo.pQueueFamilyIndices = info.queueFamilyIndices.data();
        buffInfo.sharingMode = info.queueFamilyIndices.size() == 1 ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT;
        buffInfo.size = info.size;

        Mem::Info memInfo{};

        if (info.type == Type::DEVICE_LOCAL)
            memInfo.preferred = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        else
            memInfo.preferred = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        switch(info.usage) {
            case Usage::NONE:
                break;
            case Usage::UNIFORM:
                buffInfo.usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
                break;
            case Usage::STORAGE:
                buffInfo.usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
                break;
            case Usage::INDEX:
                buffInfo.usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
                break;
            case Usage::VERTEX:
                buffInfo.usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
                break;
            default:
                throw std::runtime_error("Forgot to implement");
        }

        mem = Mem(context, memInfo, buffInfo, buff);

        //0 = host local, 1 = dev local
        type = (Type)(mem.getAllocInfo().memoryType & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    }

    size_t Buffer::getSize() {
        return mem.getSize();
    }

    VkBuffer Buffer::getBuff() {
        return buff;
    }

    VkDescriptorBufferInfo* Buffer::getDescriptorInfo() {
        return descriptorInfo.get();
    }

    void* Buffer::map() {
        if(type != Type::HOST_LOCAL) {
            ERROR("Tried to map non-host memory on buff " << this);
            return nullptr;
        }

        return mem.map();
    }

    void Buffer::unmap() {
        if(type != Type::HOST_LOCAL) {
            ERROR("Tried to umap non-host memory on buff " << this);
            return;
        }

        mem.unmap();
    }

    void Buffer::copyToBuff(uint8_t* src, size_t srcSize) {
        switch(type) {
            case Type::HOST_LOCAL:
                mem.copyToHost(src, srcSize);
                break;
            case Type::DEVICE_LOCAL:
                throw std::runtime_error("UNIMPLEMENTED");
        }
    }
}