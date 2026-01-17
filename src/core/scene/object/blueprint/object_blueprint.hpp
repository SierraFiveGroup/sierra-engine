#pragma once

#include <vector>
#include <string>
#include <tuple>
#include <cstdint>
#include <cassert>
#include <typeinfo>
#include <cstring>
#include <unordered_map>
#include <cxxabi.h>

#include "core/scene/component/loader/component_template.hpp"
#include "core/scene/component/loader/component_loader.hpp"

#include "resource_manager/resources/resources.hpp"

#include "components/inc/Drawable.hpp"

namespace Sierra {
    class ObjectBlueprint {
        friend class Object;

        public:
            struct Info {
                std::vector<uint32_t> componentCodes;
                Component::Drawable::Data drawableData;
                ComponentLoader::ComponentMap* map;
            };

            ObjectBlueprint();
            ObjectBlueprint(Info info);

            ObjectBlueprint(ObjectBlueprint&) = delete;
            ObjectBlueprint(ObjectBlueprint&&) = delete;

        protected:
            uint32_t getComponentOffset(uint32_t code);

            Component::Drawable::Data& getDrawableData();

        private:
            Component::Drawable::Data drawableData;
            ComponentLoader::ComponentMap* map;
            std::vector<uint32_t> componentCodes;
    };
}