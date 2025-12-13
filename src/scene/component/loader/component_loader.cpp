#include "component_loader.hpp"

namespace Sierra{
    ComponentTemplate ComponentLoader::loadComponent(std::string path) {
        ComponentTemplate comp{};

        comp.dlptr = dlopen(path.c_str(), RTLD_LAZY);
        //has to be lazy so it doesn't blow up when trying to resolve other shit

        if(!comp.dlptr) throw std::runtime_error("Failed to open shared object file with error: "  + (std::string)dlerror());

        comp.init = (void*(*)(void*))dlsym(comp.dlptr, "init");
        comp.update = (void(*)())dlsym(comp.dlptr, "update"); //needed?
        comp.destruct = (void(*)(void*))dlsym(comp.dlptr, "destruct");

        if( !comp.init || !comp.update || !comp.destruct ) {
            throw std::runtime_error("Not all required symbols present in shared object file: " + path);
        }

        return comp;
    }

    void ComponentLoader::unloadComponent(ComponentTemplate comp) {
        dlclose(comp.dlptr);
    }
}