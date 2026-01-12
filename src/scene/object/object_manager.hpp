#pragma once

#include <vector>

#include "scene/component/loader/component_loader.hpp"
#include "object.hpp"

namespace Sierra {
    class ObjectManager{
        public:
            ObjectManager(ComponentLoader componentLoader);

            ObjectID createObject();

        private:
            ComponentLoader componentLoader;

            uint32_t rowCount;
    };
}