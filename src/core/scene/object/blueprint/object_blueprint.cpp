#include "object_blueprint.hpp"

namespace Sierra {

    ObjectBlueprint::ObjectBlueprint() {

    }

    ObjectBlueprint::ObjectBlueprint(ComponentLoader& componentLoader) {
        loadOffsets(componentLoader);
    }

    void ObjectBlueprint::loadOffsets(ComponentLoader& componentLoader) {
        ComponentLoader::ComponentMap& compMap = componentLoader.getTemplates();

        for(std::pair<std::string, uint32_t>& name_offset : componentNames) {
            auto it = compMap.find(name_offset.first);

            assert(it != compMap.end());

            name_offset.second = it->second.getSize();
        }
    }
}