#include "object_blueprint.hpp"

namespace Sierra {

    ObjectBlueprint::ObjectBlueprint(): componentNames(), componentTemplates() {

    }

    ObjectBlueprint::ObjectBlueprint(Info info): componentNames(info.componentNames), componentTemplates() {

    }

    void ObjectBlueprint::loadOffsets(ComponentLoader& componentLoader) {
        ComponentLoader::ComponentMap& compMap = componentLoader.getTemplates();
        componentTemplates.reserve(componentNames.size());

        

        for(std::string& name : componentNames) {
            auto it = compMap.find(name);
            assert(it != compMap.end());

            componentTemplates.push_back(it->second);
        }
    }

    uint32_t ObjectBlueprint::getComponentTypeOffset(size_t typeHash, const char* typeName) {
        auto it = componentOffsetCache.find(typeHash);
        if(it != componentOffsetCache.end()) {
            return it->second;
        }

        for(int i = 0; i < componentNames.size(); i++) {
            char* realnameDemangled = abi::__cxa_demangle(typeName, NULL, NULL, NULL);

            if (strcmp(realnameDemangled, componentNames[i].c_str())) { // not here
                free(realnameDemangled);
                break;
            }

            free(realnameDemangled);
            it = componentOffsetCache.insert({typeHash, componentTemplates[i].getBlockOffset()}).first;
        }

        if(it == componentOffsetCache.end()) {
            throw std::runtime_error("Component " + (std::string)typeName + " not found in object blueprint"); // TODO maybe demangled name?
        }

        return it->second;
    }
}