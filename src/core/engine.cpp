#include "engine.hpp"

#include <iostream>


namespace Sierra {
    Engine::Engine() {
        loadComponents();
        loadRenderer();

        renderer.init({}, resManager);
    }

    void Engine::loadComponents() {
        std::filesystem::path path = SIERRA_COMPONENTS_SO_PATH;
        std::vector<std::string> componentNames;

        for(auto const& entry : std::filesystem::directory_iterator(path)) {
            componentNames.push_back(entry.path().filename());            
        }

        compLoader = ComponentLoader(componentNames);
    }

    void Engine::loadRenderer() {
        renderer = Renderer("./librenderer.so"); // make it non const at some point in the future 
    }

    Engine::~Engine() {
        renderer.cleanup();
    }
}