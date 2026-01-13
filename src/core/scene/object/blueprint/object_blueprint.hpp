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

namespace Sierra {
    class ObjectBlueprint {
        friend class Object;

        public:
            struct Info {
                std::vector<std::string> componentNames;
            };

            ObjectBlueprint();
            ObjectBlueprint(Info info);

            ObjectBlueprint(ObjectBlueprint&) = delete;
            ObjectBlueprint(ObjectBlueprint&&) = delete;

            void loadOffsets(ComponentLoader& componentLoader);
        protected:

            uint32_t getComponentTypeOffset(size_t typeHash, const char* typeName);

            std::vector<std::string> componentNames;
            std::vector<ComponentTemplate> componentTemplates; // TODO are all of these arrays eve needed?

            std::unordered_map<size_t, uint32_t> componentOffsetCache;
    };
}