#include "object_blueprint.hpp"

namespace Sierra {

    ObjectBlueprint::ObjectBlueprint(): componentCodes(), map() {

    }

    ObjectBlueprint::ObjectBlueprint(Info info): componentCodes(info.componentCodes), 
     map(info.map), drawableData(info.drawableData) {

    }

    uint32_t ObjectBlueprint::getComponentOffset(uint32_t code) {
        return (*map)[code].getBlockOffset();
    }

    Component::Drawable::Data& ObjectBlueprint::getDrawableData() {
        return drawableData;
    }
}