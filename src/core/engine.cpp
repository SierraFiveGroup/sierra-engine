#include "engine.hpp"

#include "components/inc/Transform3D.hpp"

namespace Sierra {
    Engine::Engine(): window("Sierra ma balls", {800, 600}, API::vulkan), taskManager(), resManager({&taskManager, renderer.getLoadFunc(), 3}) {
        loadComponents();
        loadRenderer();

        taskManager.printTasks();
        taskManager.start();
        while(!taskManager.isFinished());

        Scene scene = Scene({&compLoader, &resManager, 32});
        ObjectBlueprint bp = ObjectBlueprint({{SIERRA_COMPONENT_TRANSFORM_3D}, {0, 1}, &compLoader.getTemplates()});
        Object* obj = scene.createObject(bp);

        Component::Transform3D* transform = obj->getComponent<Component::Transform3D>(SIERRA_COMPONENT_TRANSFORM_3D);
        transform->setPos({1, 1, 1});

        while(!window.shouldClose()) {
            scene.update();

        }
    }

    void Engine::loadComponents() {
        std::filesystem::path path = SIERRA_COMPONENTS_SO_PATH;
        std::vector<std::string> componentNames;

        for(auto const& entry : std::filesystem::directory_iterator(path)) {
            componentNames.push_back(entry.path());
        }

        new (&compLoader) ComponentLoader(componentNames);
    }

    void Engine::run() {
            
    }

    void Engine::loadRenderer() {
        renderer = Renderer("./librenderer." SIERRA_SO_EXT); // make it non const at some point in the future 

        Renderer::Configuration conf{};
        conf.window = &window;
        conf.resManager = &resManager;
        conf.shaderPaths = {
            {"test_shaders/vk/model.vert.spv", "test_shaders/vk/model.frag.spv"}
        };
        conf.taskManager = &taskManager;

        renderer.init(conf);

    }

    Engine::~Engine() {
        renderer.cleanup();
    }
}