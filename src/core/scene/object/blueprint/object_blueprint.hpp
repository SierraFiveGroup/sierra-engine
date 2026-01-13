#pragma once

#include <vector>
#include <string>
#include <tuple>
#include <cstdint>
#include <cassert>

#include "core/scene/component/loader/component_template.hpp"
#include "core/scene/component/loader/component_loader.hpp"

namespace Sierra {
    class ObjectBlueprint {
        public:
            ObjectBlueprint();
            ObjectBlueprint(ComponentLoader& componentLoader);

            void loadOffsets(ComponentLoader& componentLoader);
        private:
            std::vector<std::pair<std::string, uint32_t>> componentNames; // name_offset pair //split into 2 vectors so initialization is faster
    };
}