#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_set>

#include "resources/resources.hpp"
#include <tasks/manager.hpp>

namespace Sierra {
    class ResourceManager {
        public:
            typedef uint64_t ResID;

            enum class ResourceType {
                IMAGE,
                MODEL,
                SOUND
            };

            struct LoadInfo {
                ResourceType type;
                std::string path;
            };

            struct LoadPacket {
                std::vector<LoadInfo> loadInfos;
            };

            typedef std::vector<Res::ResourceAny> (*LoadFunc)(LoadPacket);

            ResourceManager();
            ResourceManager(TaskManager& taskManager, LoadFunc loadfunc);

            std::vector<ResID> loadResources(LoadPacket& packet);

            bool isLoaded(ResID res);
            bool unload(ResID res);

        private:
            std::unordered_map<ResID, Res::ResourceAny> resIDs; // first use as base to find out type then handle appropriately

            TaskManager* taskManager;
            LoadFunc loadFunc;
    };
}
