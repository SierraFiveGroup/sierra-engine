#include "component.hpp"

namespace Sierra::Component {

    uint32_t ComponentBase::_INTERNAL_blockOffset = 0;

    uint32_t ComponentBase::getID() {
        return _INTERNAL_blockOffset;
    }
}