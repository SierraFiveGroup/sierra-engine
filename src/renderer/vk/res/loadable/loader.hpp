#pragma once

#include "res/loadable/model/vk_model.hpp"
#include "res/loadable/texture/vk_texture.hpp"
#include "resource_manager/resource_manager.hpp"

namespace Sierra::vlk {
    class Loader {
        public:
            Loader();
            Loader(Context& context);

            std::vector<Res::ResourceAny> load(ResourceManager::_RendererLoadPacket& packet);
        private:
            Res::ResourceAny loadModel(ResourceManager::_RendererLoadInfo& packet, TaskManager* taskManager);
            Res::ResourceAny loadTexture(ResourceManager::_RendererLoadInfo& packet, TaskManager* taskManager);

            Context* context;
            MemoryManager memManager;
            MemLoader memLoader;
    };
}