#pragma once

#include <filesystem>
#include <fstream>

#include "scene/component/component.hpp"
#include "scene/component/loader/component_loader.hpp"

#include "renderer.hpp"

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

            Renderer renderer;
            ResourceManager resManager;
            ComponentLoader compLoader;
    };
}