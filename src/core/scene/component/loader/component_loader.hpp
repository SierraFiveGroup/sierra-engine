#pragma once

#ifndef _WIN32
#include <dlfcn.h>
#endif

#include <string>
#include <vector>
#include <stdexcept>

#include "component_template.hpp"

#define SIERRA_COMPONENTS_SO_PATH "components/so"

namespace Sierra {
    class ComponentLoader {
        public:
            ComponentLoader() = default;
            ComponentLoader(std::vector<std::string> componentNames);
            ~ComponentLoader();

            ComponentLoader(ComponentLoader&) = delete;
            void operator=(ComponentLoader&) = delete;

            void operator=(ComponentLoader&&);

            std::vector<ComponentTemplate> getTemplates();
            size_t getBlockSize();
        private:
            ComponentTemplate loadComponent(std::string path);
            void unloadComponent(ComponentTemplate comp);

            size_t blockSize;

            std::vector<ComponentTemplate> templates;
    };
};