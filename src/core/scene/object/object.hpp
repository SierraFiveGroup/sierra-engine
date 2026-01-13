#pragma once

#include <cstdint>

#include "../component/component.hpp"

namespace Sierra{
    class Object {
        friend class Scene;
        public:
            Object();

        protected:
            Object(uint8_t* row);

            template<typename T>
            T* getComponent(uint32_t compID);

            Component* getComponentBase(uint32_t compID);


        private:
            uint8_t* row;
    };

    #include "object.tpp"
}