#include "object.hpp"

namespace Sierra {
    Object::Object(): row(nullptr) {

    }

    Object::Object(uint8_t* row): row(row) {

    }

    Component* Object::getComponentBase(uint32_t compID) {
        return getComponent<Component>(compID);
    }
}