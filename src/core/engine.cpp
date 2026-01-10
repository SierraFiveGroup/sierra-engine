#include "engine.hpp"

#include <iostream>


namespace Sierra {
    Engine::Engine() {
        loadComponents();
    }

    void Engine::loadComponents() {
        std::filesystem::path path = SIERRA_COMPONENTS_SO_PATH;
        std::vector<std::string> componentNames;

        for(auto const& entry : std::filesystem::directory_iterator(path)) {
            componentNames.push_back(entry.path().filename());            
        }

        compLoader = ComponentLoader(componentNames);
    }
}