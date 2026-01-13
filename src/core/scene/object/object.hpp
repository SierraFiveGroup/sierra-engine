#pragma once

#include <cstdint>

#include "../component/component.hpp"
#include "blueprint/object_blueprint.hpp"

namespace Sierra{
    class Object {
        friend class Scene;

        public:
            Object();

            Object(Object&) = delete;
            Object(Object&&) = delete;

            ~Object();

            template<typename T>
            T* getComponent();

        protected:
            Object (ObjectBlueprint& blueprint, uint8_t* row);

        private:
            void constructComponents();

            ObjectBlueprint* blueprint;

            uint8_t* row;
    };

    #include "object.tpp"
}