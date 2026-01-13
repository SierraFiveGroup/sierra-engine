#include "component_loader.hpp"

namespace Sierra{

    ComponentLoader::ComponentLoader(std::vector<std::string> componentNames): blockSize(0), templates() {
        templates.reserve(componentNames.size());
        for(std::string& name : componentNames) {
            ComponentTemplate temp = loadComponent(name);
            temp.setBlockOffset(blockSize);
            blockSize += temp.getSize();

            templates[temp.name] = temp;
        }
    }

    ComponentTemplate ComponentLoader::loadComponent(std::string path) {
        ComponentTemplate comp{};

        std::vector<std::string> trials;

        // Prefer build-time components directory relative to current working directory
        try {
            std::filesystem::path cwd = std::filesystem::current_path();
            std::filesystem::path preferred = cwd / SIERRA_COMPONENTS_SO_PATH / path;
            trials.push_back(preferred.string());
        } catch(...) {}

        // If user passed a relative or absolute path with a separator, try it directly
        if (path.find('/') != std::string::npos) {
            trials.push_back(path);
        }

        // Finally, try the bare name (relies on system loader search paths)
        trials.push_back(path);

        for (const auto& candidate : trials) {
            comp.dlptr = dlopen(candidate.c_str(), RTLD_LAZY | RTLD_GLOBAL);
            if (comp.dlptr) {
                break;
            }
        }

        if(!comp.dlptr) throw std::runtime_error("Failed to open shared object file with error: "  + (std::string)dlerror());

        comp.init = (void*(*)(void*))dlsym(comp.dlptr, "init");
        comp.getSize = (size_t(*)())dlsym(comp.dlptr, "getSize"); 
        comp.setBlockOffset = (void(*)(size_t))dlsym(comp.dlptr, "setBlockOffset"); 
        comp.getBlockOffset = (size_t (*)())dlsym(comp.dlptr, "getBlockOffset"); ;
        comp.destruct = (void(*)(void*))dlsym(comp.dlptr, "destruct");

        comp.name = path.substr(path.find_last_of('/') + 1); // strip the path
        comp.name = comp.name.substr(3, comp.name.find_last_of('.') - 3); // strip the extension and the "lib" part

        if( !comp.init || !comp.getSize || !comp.destruct || !comp.getBlockOffset || !comp.setBlockOffset) {
            throw std::runtime_error("Not all required symbols present in shared object file: " + path);
        }

        return comp;
    }

    ComponentLoader::ComponentMap& ComponentLoader::getTemplates() {
        return templates;
    }

    size_t ComponentLoader::getBlockSize() {
        return blockSize;
    }

    void ComponentLoader::unloadComponent(ComponentTemplate comp) {
        dlclose(comp.dlptr);
    }
    
    ComponentLoader::ComponentLoader(ComponentLoader&& other) {
        templates = std::move(other.templates);
        blockSize = other.blockSize;
    }

    void ComponentLoader::operator=(ComponentLoader&& other) {
        templates = std::move(other.templates);
        blockSize = other.blockSize;
    }

    ComponentLoader::~ComponentLoader() {
        for(auto& temp : templates) {
            unloadComponent(temp.second);
        }
    }

}