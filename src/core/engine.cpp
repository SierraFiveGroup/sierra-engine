#include "engine.hpp"

#include "components/inc/Transform3D.hpp"

namespace Sierra {
    Engine::Engine(): window("Sierra ma balls", {800, 600}, API::vulkan), taskManager(), resManager({&taskManager, renderer.getLoadFunc(), 3}) {
        loadComponents();
        loadRenderer();

        Scene scene = Scene({&compLoader, &resManager, 32});
        ObjectBlueprint bp = ObjectBlueprint({{SIERRA_COMPONENT_TRANSFORM_3D}, {0, 1}, &compLoader.getTemplates()});
        Object obj = scene.createObject(bp);

        Component::Transform3D* transform = obj.getComponent<Component::Transform3D>(SIERRA_COMPONENT_TRANSFORM_3D);
        transform->setPos({1, 1, 1});

        scene.update();
        scene.update();
        scene.update();
        
    }

    void Engine::loadComponents() {
        std::filesystem::path path = SIERRA_COMPONENTS_SO_PATH;
        std::vector<std::string> componentNames;

        for(auto const& entry : std::filesystem::directory_iterator(path)) {
            componentNames.push_back(entry.path());
        }

        new (&compLoader) ComponentLoader(componentNames);
    }

    void Engine::loadRenderer() {
        #ifdef __linux__
        renderer = Renderer("./librenderer.so"); // make it non const at some point in the future 
        #elif __APPLE__
        renderer = Renderer("./librenderer.dylib");
        // TODO: Windows
        #endif

        Renderer::Configuration conf{};
        conf.window = &window;

        renderer.init(conf, resManager);

    }

    Engine::~Engine() {
        resManager.cleanup();
        renderer.cleanup();
    }
}