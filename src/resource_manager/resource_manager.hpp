#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_set>

#include "resources/resources.hpp"
#include "tasks/manager.hpp"
#include "components/inc/Transform3D.hpp"
#include "rendering/render_state_manager.hpp"

namespace Sierra {
    class ResourceManager {
        public:


            struct LoadInfo{
                std::string path;
                Res::Type type;
            };

            struct LoadPacket {
                std::vector<LoadInfo> loadInfos;
            };

            struct _RendererLoadInfo {
                union BaseResource {
                    Sierra::Model* model;
                    Sierra::Texture* texture;
                } baseRes;

                Res::Type type;
            };

            struct _RendererLoadPacket {
                std::vector<_RendererLoadInfo> loadInfos;

                TaskManager* taskManager;
            };

            typedef std::vector<Res::ResourceAny> (*LoadFunc)(_RendererLoadPacket);

            struct Info{
                TaskManager* taskManager;
                LoadFunc loadFunc;

                uint32_t framebufferCount;
            };


            ResourceManager();
            ResourceManager(Info info);

            ResourceManager(ResourceManager&) = delete;

            ResourceManager(ResourceManager&&);
            void operator=(ResourceManager&&);

            void cleanup();

            std::vector<Res::ResID> loadResources(LoadPacket& packet);

            RenderStateManager& getRenderManager();


            bool isLoaded(Res::ResID res);
            bool unload(Res::ResID res);

        private:
            struct BaseResourceBuffers {
                std::vector<Sierra::Model> models;
                std::vector<Sierra::Texture> textures;
            };

            _RendererLoadInfo loadModel(BaseResourceBuffers& baseBuffers, LoadInfo loadInfo);
            _RendererLoadInfo loadTexture(BaseResourceBuffers& baseBuffers, LoadInfo loadInfo);

            std::unordered_map<Res::ResID, Res::ResourceAny> resIDs; // first use as base to find out type then handle appropriately

            std::vector<BaseResourceBuffers> baseResBuffers;

            TaskManager* taskManager;
            LoadFunc loadFunc;

            uint64_t resIDInc;

            RenderStateManager renderManager;
    };
}
