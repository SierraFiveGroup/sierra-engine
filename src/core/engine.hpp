#pragma once

#include <filesystem>
#include <fstream>
#include <thread>

#include "scene/component/component.hpp"
#include "scene/component/loader/component_loader.hpp"
#include "scene/object/blueprint/object_blueprint.hpp"
#include "common/component_names.hpp"

#include "renderer.hpp"
#include "window/window.hpp"
#include "core/scene/scene.hpp"

namespace Sierra{
    class Engine{
        public:
            Engine();

            Engine(Engine&) = delete;
            Engine(Engine&&) = delete;

            ~Engine();

            void run();

        private:
            void loadComponents();
            void loadRenderer();

            Window window;
            Renderer renderer;
            ResourceManager resManager;
            TaskManager taskManager;
            ComponentLoader compLoader;
    };
}