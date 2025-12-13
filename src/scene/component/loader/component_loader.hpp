#pragma once

#include <dlfcn.h>

#include <string>
#include <stdexcept>

#include "component_template.hpp"

namespace Sierra {
    class ComponentLoader {
        public:
            static ComponentTemplate loadComponent(std::string path);
            static void unloadComponent(ComponentTemplate comp);
    };
};