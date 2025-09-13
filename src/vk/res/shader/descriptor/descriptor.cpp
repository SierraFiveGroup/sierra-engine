#include "descriptor.hpp"

namespace Sierra::vlk {

    Descriptor::Descriptor(): name(), type() {

    }

    Descriptor::Descriptor(std::string name, VkDescriptorType type): name(name), type(type) {

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
}