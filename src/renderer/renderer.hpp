#pragma once

#include <string>
#include <stdexcept>

#ifdef __linux__
#include <dlfcn.h>
#endif

#include "logging/logger.hpp"
#include "resource_manager.hpp"

namespace Sierra{

    class Renderer {
        public:
            struct Error {
                int code;
                std::string message;
            };

            //Just stating they exist, no idea what they'll look like
            struct RenderState {

            };

            struct Configuration {

            };

            Renderer(): _init(), _update(), _cleanup(), _setConfiguration(), _getConfiguration(), _getError(), dlptr() {

            }

            Renderer(std::string objectPath) {
                loadLibrary(objectPath);
            }

            int init(Configuration configuration, ResourceManager& resourceManager) {
                return _init(configuration, resourceManager);
            }

            int update() {
                return _update();
            }

            int cleanup() {
                int ret = _cleanup();

                if(dlptr)
                    dlclose(dlptr);

                return ret;
            }

            int setConfiguration(Configuration configuration) {
                return _setConfiguration(configuration);
            }

            Configuration getConfiguration() {
                return _getConfiguration();
            }

            void* loadResource(ResourceManager::LoadInfo loadInfo) {
                *(char*)(0x0); // kill yourself
                return 0;
            }

            Error getError(/*int?*/) {
                return _getError();
            }

        private:
            int (*_init)(Configuration, ResourceManager&);
            int (*_update)(void);
            int (*_cleanup)(void);

            int (*_setConfiguration)(Configuration);
            Configuration (*_getConfiguration)(void);

            Error (*_getError)(void);

            void* (*_loadResource)(ResourceManager::LoadInfo);

            void *dlptr;

            void loadLibrary(std::string libName) {
                dlptr = dlopen(libName.c_str(), RTLD_LAZY);
                if(!dlptr) throw std::runtime_error("Failed to open shared object file with error: "  + (std::string)dlerror());


                _init = (int(*)(Configuration, ResourceManager&))dlsym(dlptr, "init");
                _update = (int(*)())dlsym(dlptr, "update");
                _cleanup = (int(*)())dlsym(dlptr, "cleanup");
                _setConfiguration = (int(*)(Configuration))dlsym(dlptr, "setConfiguration");
                _getConfiguration = (Configuration(*)())dlsym(dlptr, "getConfiguration");
                _getError = (Error(*)())dlsym(dlptr, "getError");
                _loadResource = (void*(*)(ResourceManager::LoadInfo))dlsym(dlptr, "loadResource");



                if(!_init || !_update || !_cleanup || !_setConfiguration || !_getConfiguration || !_getError) {
                    throw std::runtime_error("Failed to load all the renderer functions from the shared object file");
                }
            }

    };
}