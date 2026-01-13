#include "engine.hpp"

#include "components/inc/Transform3D.hpp"

namespace Sierra {
    Engine::Engine(): window("Sierra ma balls", {800, 600}, API::vulkan), taskManager() {
        loadComponents();
        loadRenderer();

        Scene scene = Scene({&compLoader, 32});
        ObjectBlueprint bp = ObjectBlueprint({{SIERRA_COMPONENT_TRANSFORM_3D}});
        bp.loadOffsets(compLoader);
        Object obj = scene.createObject(bp);

        Transform3D* transform = obj.getComponent<Transform3D>();
        transform->setPos({1, 1, 1});
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