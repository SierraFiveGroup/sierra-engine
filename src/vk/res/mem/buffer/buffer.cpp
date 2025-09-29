#include "buffer.hpp"

namespace Sierra::vlk {
    Buffer::Buffer(): mem(), buff() {

    }

    Buffer::Buffer(Context& context, Info& info) {
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
}