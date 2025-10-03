#pragma once

#include <future>
#include <vector>
#include <fstream>

#include "scene/tasks/task.hpp"

namespace Sierra {
    class LoadableResource {
        public:
            LoadableResource(std::string resourcePath);
            
            std::future<std::vector<uint8_t>> getDat();
            Task getTask();
        protected:
            struct AsyncDat {
                std::promise<std::vector<uint8_t>> contents;
                std::string filePath;
            };

            
        private:
            static void asyncLoad(std::shared_ptr<void> asyncDat, std::function<void()> finished);
            
            std::shared_ptr<AsyncDat> dat;
            Task task;

    };
}