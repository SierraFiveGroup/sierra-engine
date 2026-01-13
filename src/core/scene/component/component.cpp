#include "component.hpp"

namespace Sierra {

    uint32_t Component::_INTERNAL_blockOffset = 0;

    uint32_t Component::getID() {
        return _INTERNAL_blockOffset;
    }
}