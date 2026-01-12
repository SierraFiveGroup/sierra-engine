#pragma once

#include <filesystem>
#include <fstream>

#include "scene/component/component.hpp"
#include "scene/component/loader/component_loader.hpp"

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

        private:
            void loadComponents();
            void loadRenderer();

            Window window;
            Renderer renderer;
            ResourceManager resManager;
            ComponentLoader compLoader;
            TaskManager taskManager;
    };
}