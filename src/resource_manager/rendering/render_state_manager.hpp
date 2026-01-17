#pragma once

#include <optional>
#include <tuple>
#include <atomic>

#include "../resources/resources.hpp"

#include "components/inc/Transform3D.hpp"

namespace Sierra {
    class RenderStateManager {
        friend class ResourceManager;
        public:
            struct RenderBatch {
                Res::ResID shaderID;
                Res::ResID modelID;

                std::vector<Component::Transform3D> instanceData;
            };

            struct RenderPacket {
                std::vector<RenderBatch> batches;
            };

            RenderStateManager();

            std::optional<RenderPacket*> getEmptyPacket();
            std::optional<RenderPacket*> getFullPacket();
        protected:
            RenderStateManager(uint32_t framebufferCount);
        private:
            //bool indicates whether its used
            std::vector<std::pair<RenderPacket, std::atomic_bool>> renderPackets; 
            uint32_t nextPacketIndex;
    };
}