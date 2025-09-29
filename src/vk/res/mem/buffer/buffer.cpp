#include "buffer.hpp"

namespace Sierra::vlk {
    Buffer::Buffer(): mem(), buff(), type() {

    }

    Buffer::Buffer(Context& context, Info& info): type(info.type) {
        createBuff(context, info);
    }

    void Buffer::createBuff(Context& context, Info& info) {
        VkBufferCreateInfo buffInfo{};
        buffInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffInfo.usage = info.usage;
        buffInfo.queueFamilyIndexCount = info.queueFamilyIndices.size();
        buffInfo.pQueueFamilyIndices = info.queueFamilyIndices.data();
        buffInfo.size = info.size;

        Mem::Info memInfo{};

        if (info.type == Type::DEVICE_LOCAL)
            memInfo.preferred = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        else
            memInfo.preferred = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
            

        mem = Mem(context, memInfo, buffInfo, buff);
    }

    size_t Buffer::getSize() {
        return mem.getAllocInfo().size;
    }

    VkBuffer Buffer::getBuff() {
        return buff;
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
                throw new std::runtime_error("UNIMPLEMENTED");
        }
    }
}