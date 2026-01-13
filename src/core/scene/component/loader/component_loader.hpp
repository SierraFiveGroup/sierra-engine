#pragma once

#ifndef _WIN32
#include <dlfcn.h>
#endif

#include <string>
#include <vector>
#include <stdexcept>
#include <filesystem>
#include <unordered_map>

#include "component_template.hpp"
#include "common/logging/logger.hpp"

#define SIERRA_COMPONENTS_SO_PATH "components/so"

namespace Sierra {
    class ComponentLoader {
        public:
            typedef std::unordered_map<std::string, ComponentTemplate> ComponentMap;

            ComponentLoader() = default;
            ComponentLoader(std::vector<std::string> componentNames);
            ~ComponentLoader();

            ComponentLoader(ComponentLoader&) = delete;

            ComponentLoader(ComponentLoader&&);
            void operator=(ComponentLoader&& other);

            ComponentMap& getTemplates();
            size_t getBlockSize();
        private:
            ComponentTemplate loadComponent(std::string path);
            void unloadComponent(ComponentTemplate comp);

            size_t blockSize;

            ComponentMap templates;
    };
};