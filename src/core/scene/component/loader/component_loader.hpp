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

#ifdef __linux__
#define SIERRA_COMPONENTS_SO_EXT ".so"
#define SIERRA_COMPONENTS_SO_EXT_LEN 3
#elif __APPLE__
#define SIERRA_COMPONENTS_SO_EXT ".dylib"
#define SIERRA_COMPONENTS_SO_EXT_LEN 6
#elif _WIN32
#define SIERRA_COMPONENTS_SO_EXT ".dll"
#define SIERRA_COMPONENTS_SO_EXT_LEN 4
#endif

namespace Sierra {
    class ComponentLoader {
        public:
            typedef std::unordered_map<uint32_t, ComponentTemplate> ComponentMap;

            ComponentLoader() = default;
            ComponentLoader(std::vector<std::string> componentNames);
            ~ComponentLoader();

            ComponentLoader(ComponentLoader&) = delete;

            ComponentLoader(ComponentLoader&&) = delete;

            ComponentMap& getTemplates();
            size_t getBlockSize();
        private:
            ComponentTemplate loadComponent(std::string path);
            void unloadComponent(ComponentTemplate comp);

            size_t blockSize;

            ComponentMap templates;
    };
};