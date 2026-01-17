#pragma once

#include "common/util/structures/hive.hpp"
#include "core/scene/component/loader/component_loader.hpp"
#include "resource_manager/resource_manager.hpp"
#include "common/logging/logger.hpp"
#include "object/object.hpp"

#include "components/inc/Transform3D.hpp"

#include <tuple>
#include <unordered_map>
#include <vector>
#include <utility>
#include <functional>

#include "common/util/hash.hpp"

namespace Sierra {
    class Scene {
        public:
            struct pair_hash {
                template <class T1, class T2>
                std::size_t operator () (const std::pair<T1,T2> &pair) const {
                    auto hash1 = std::hash<T1>{}(pair.first);
                    auto hash2 = std::hash<T2>{}(pair.second);
                    return hash1 ^ hash2;
                }
            };

            struct SceneInfo {
                ComponentLoader *compLoader;
                ResourceManager *resManager;
                uint32_t objCountPerBlock;
            };

            Scene(); 
            Scene(SceneInfo info); 

            Object* createObject(ObjectBlueprint& objBlueprint);
            void eraseObject(Object obj);

            void update();
        
        private:

            void initComponents(SceneInfo& info);

            void submitRenderPacket();

            Hive<uint8_t> objects;

            ResourceManager* resManager;
    };
}