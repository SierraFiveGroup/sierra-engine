#pragma once

#include "common/util/structures/hive.hpp"
#include "core/scene/component/loader/component_loader.hpp"
#include "common/logging/logger.hpp"

namespace Sierra {
    class Scene {
        public:
            struct SceneInfo {
                ComponentLoader *compLoader;
                uint32_t objCountPerBlock;
            };

            Scene(); 
            Scene(SceneInfo info); 
        private:
            void initComponents(SceneInfo& info);

            Hive<uint8_t> objects;
    };
}