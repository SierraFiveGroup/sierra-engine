#include "engine.hpp"

#include <iostream>


namespace Sierra {
    Engine::Engine(): window("Sierra ma balls", {800, 600}, API::vulkan), taskManager() {
        loadComponents();
        loadRenderer();

        resManager = ResourceManager(taskManager, renderer.getLoadFunc());

        ResourceManager::LoadPacket packet{};
        packet.loadInfos = {
            ResourceManager::LoadInfo("lepotec.jpg", Res::Type::TEXTURE)
        };

        resManager.loadResources(packet);
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

        Renderer::Configuration conf{};
        conf.window = &window;

        renderer.init(conf, resManager);

    }

    Engine::~Engine() {
        resManager.cleanup();
        renderer.cleanup();
    }
}