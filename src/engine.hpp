#pragma once

#include <filesystem>
#include <fstream>

#include "scene/component/component.hpp"
#include "scene/component/loader/component_loader.hpp"

namespace Sierra{
    class Engine{
        public:
            Engine();

        private:
            void loadComponents();
            ComponentLoader compLoader;
    };
}