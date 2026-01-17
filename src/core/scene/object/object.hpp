#pragma once

#include <cstdint>

#include "../component/component.hpp"
#include "blueprint/object_blueprint.hpp"
#include "common/component_names.hpp"

namespace Sierra{
    class Object {
        friend class Scene;

        public:
            Object();

            Object(Object&) = delete;
            Object(Object&&) = delete;

            ~Object();

            template<typename T>
            T* getComponent(uint32_t code);

            Res::ResID getModelID();
            Res::ResID getShaderID();

        protected:
            Object (ObjectBlueprint& blueprint, uint8_t* row);

        private:
            void constructComponents();
            void initDrawable();

            ObjectBlueprint* blueprint;

            Res::ResID shaderID; // todo figure out how to set normally

            uint8_t* row;
    };

    #include "object.tpp"
}