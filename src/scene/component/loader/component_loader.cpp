#include "component_loader.hpp"

namespace Sierra{

    ComponentLoader::ComponentLoader(std::vector<std::string> componentNames): blockSize(0), templates() {
        std::string envStr = (std::string)"LD_LIBRARY_PATH=" + SIERRA_COMPONENTS_SO_PATH;
        putenv((char*)envStr.c_str());

        templates.reserve(componentNames.size());
        for(std::string& name : componentNames) {
            templates.emplace_back(
                loadComponent(name)
            );
            templates.back().setBlockOffset(blockSize);
            blockSize += templates.back().getSize();
        }
    }

    ComponentTemplate ComponentLoader::loadComponent(std::string path) {
        ComponentTemplate comp{};

        comp.dlptr = dlopen(path.c_str(), RTLD_NOW | RTLD_GLOBAL);

        if(!comp.dlptr) throw std::runtime_error("Failed to open shared object file with error: "  + (std::string)dlerror());

        comp.init = (void*(*)(void*))dlsym(comp.dlptr, "init");
        comp.getSize = (size_t(*)())dlsym(comp.dlptr, "getSize"); 
        comp.setBlockOffset = (void(*)(size_t))dlsym(comp.dlptr, "setBlockOffset"); 
        comp.destruct = (void(*)(void*))dlsym(comp.dlptr, "destruct");

        if( !comp.init || !comp.getSize || !comp.destruct ) {
            throw std::runtime_error("Not all required symbols present in shared object file: " + path);
        }

        return comp;
    }

    std::vector<ComponentTemplate> ComponentLoader::getTemplates() {
        return templates;
    }

    size_t ComponentLoader::getBlockSize() {
        return blockSize;
    }

    void ComponentLoader::unloadComponent(ComponentTemplate comp) {
        dlclose(comp.dlptr);
    }

    void ComponentLoader::operator=(ComponentLoader&&) {
        templates = std::move(templates);
    }

    ComponentLoader::~ComponentLoader() {
        for(ComponentTemplate& temp : templates) {
            unloadComponent(temp);
        }
    }

}