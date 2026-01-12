#include "engine.hpp"

#include <iostream>


namespace Sierra {
    Engine::Engine(): window("Sierra ma balls", {800, 600}, API::vulkan), taskManager() {
        loadComponents();
        loadRenderer();

        Scene scene = Scene({&compLoader, 32});
    }

    void Engine::loadComponents() {
        std::filesystem::path path = SIERRA_COMPONENTS_SO_PATH;
        std::vector<std::string> componentNames;

        for(auto const& entry : std::filesystem::directory_iterator(path)) {
            componentNames.push_back(entry.path());
        }

        compLoader = ComponentLoader(componentNames);
    }

    void Engine::loadRenderer() {
        renderer = Renderer("./librenderer.so"); // make it non const at some point in the future 

        Renderer::Configuration conf{};
        conf.window = &window;

        renderer.init(conf, resManager);

    }

    Engine::~Engine() {
        resManager.cleanup();
        renderer.cleanup();
    }
}