#include "descriptor.hpp"

namespace Sierra::vlk {

    Descriptor::Descriptor(): name(), type(), index(-1) {

    }

    Descriptor::Descriptor(std::string name, VkDescriptorType type): name(name), type(type), index(-1) {

    }

    VkWriteDescriptorSet Descriptor::getWrite() {
        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet = set;
        write.dstBinding = index;
        write.dstArrayElement = 0;
        write.descriptorCount = 1;
        write.descriptorType = type;

        return write;
    }

    VkWriteDescriptorSet Descriptor::getWriteBuffer(Buffer& buffer) {
        VkWriteDescriptorSet write = getWrite();
        write.pBufferInfo = buffer.getDescriptorInfo();

        return write;
    }

    VkWriteDescriptorSet Descriptor::getWriteImage(Image& buffer) {
        throw new std::runtime_error("UNIMPLEMENTED");
    }

    void Descriptor::setIndex(uint32_t index) {
        this->index = index;
    }

    std::string Descriptor::getName() {
        return name;
    }

    VkDescriptorType Descriptor::getType() {
        return type;
    }

    uint32_t Descriptor::getIndex() {
        return index;
    }

    void Descriptor::setDescriptorSet(VkDescriptorSet set) {
        this->set = set;
    }
}