#pragma once

#include <string>
#include <stdexcept>

#ifndef __WIN32
#include <dlfcn.h>
#endif

#include "logging/logger.hpp"
#include "resource_manager.hpp"
#include "window/window.hpp"

#ifdef __linux__
#define SIERRA_SO_EXT "so"
#elif  __APPLE__
#define SIERRA_SO_EXT "dylib"
#else
#error "Unsupported platform"
#endif

namespace Sierra{

    class Renderer {
        public:
            typedef std::pair<std::string, std::string> ShaderPair;

            struct Error {
                int code;
                std::string message;
            };

            struct ObjectBatch {
                Res::ResID shaderID;
                Res::ResID modelID;

                std::vector<Component::Transform3D::Data> instanceData;
            };

            struct RenderState {

            };

            struct Configuration {
                Window* window;
                ResourceManager* resManager;
                TaskManager* taskManager;
                std::vector<ShaderPair> shaderPaths; // the rationale here is that compute shaders and such wont be written by user, thus can all be predefined
            };

            Renderer(): _init(), _update(), _cleanup(), _setConfiguration(), _getConfiguration(), _getError(), dlptr() {

            }

            Renderer(std::string objectPath) {
                loadLibrary(objectPath);
            }

            int init(Configuration configuration) {
                return _init(configuration);
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

            Error getError(/*int?*/) {
                return _getError();
            }

            ResourceManager::LoadFunc getLoadFunc() {
                return _loadFunc;
            }

        private:
            int (*_init)(Configuration);
            int (*_update)(void);
            int (*_cleanup)(void);

            int (*_setConfiguration)(Configuration);
            Configuration (*_getConfiguration)(void);

            Error (*_getError)(void);

            ResourceManager::LoadFunc _loadFunc;

            void *dlptr;

            void loadLibrary(std::string libName) {
                dlptr = dlopen(libName.c_str(), RTLD_LAZY);
                if(!dlptr) throw std::runtime_error("Failed to open shared object file with error: "  + (std::string)dlerror());


                _init = (int(*)(Configuration))dlsym(dlptr, "init");
                _update = (int(*)())dlsym(dlptr, "update");
                _cleanup = (int(*)())dlsym(dlptr, "cleanup");
                _setConfiguration = (int(*)(Configuration))dlsym(dlptr, "setConfiguration");
                _getConfiguration = (Configuration(*)())dlsym(dlptr, "getConfiguration");
                _getError = (Error(*)())dlsym(dlptr, "getError");
                _loadFunc = (ResourceManager::LoadFunc)dlsym(dlptr, "loadResources");



                if(!_init || !_update || !_cleanup || !_setConfiguration || !_getConfiguration || !_getError || !_loadFunc) {
                    throw std::runtime_error("Failed to load all the renderer functions from the shared object file");
                }
            }

    };
}