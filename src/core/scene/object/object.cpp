#include "object.hpp"

namespace Sierra {
    Object::Object(): row(nullptr) {

    }

    Object::Object(ObjectBlueprint& blueprint, uint8_t* row): row(row), blueprint(&blueprint) {
        constructComponents();
        initDrawable();
    }

    void Object::constructComponents() {
        for(uint32_t& code : blueprint->componentCodes) {
            ComponentTemplate& temp = (*blueprint->map)[code];

            temp.init(row + temp.getBlockOffset());
        }


    }

    void Object::initDrawable() { // by having drawable have known memory layout we can "safely" do this
        //also the reason we cant just construct this shi is cause we arent linking it
        uint32_t drawableOffset = blueprint->getComponentOffset(SIERRA_COMPONENT_DRAWABLE);

        const uint32_t datOffset = Component::PrivateAccess::getDatOffset<Component::Drawable>(); // typeinfo wont fucking work so we have to define a few base 
        //components and constant offsets

        memcpy(row + drawableOffset + datOffset, &blueprint->getDrawableData() + datOffset, sizeof(Component::Drawable::Data));
    }


    Res::ResID Object::getModelID() {
        return blueprint->getDrawableData().modelID;
    }

    Res::ResID Object::getShaderID() {
        return blueprint->getDrawableData().shaderID;
    }
    
    Object::~Object() {
        for(uint32_t& code : blueprint->componentCodes) {
            ComponentTemplate& temp = (*blueprint->map)[code];

            temp.destruct(row + temp.getBlockOffset());
        }
    }
}