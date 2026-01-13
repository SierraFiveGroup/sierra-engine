#include "object.hpp"

namespace Sierra {
    Object::Object(): row(nullptr) {

    }

    Object::Object(ObjectBlueprint& blueprint, uint8_t* row): row(row), blueprint(&blueprint) {
        constructComponents();
    }

    void Object::constructComponents() {
        for(ComponentTemplate& temp : blueprint->componentTemplates) {
            temp.init(row + temp.getBlockOffset());
        }
    }

    Object::~Object() {
        for(ComponentTemplate& temp : blueprint->componentTemplates) {
            temp.destruct(row + temp.getBlockOffset());
        }
    }
}