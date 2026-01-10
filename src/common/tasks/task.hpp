#pragma once

#include <functional>
#include <future>

#include <logging/logger.hpp>

namespace Sierra {
    class Task {
        public:
            friend class TaskManager;
            enum class Stage {
                PRE_LOAD,
                LOAD, // loading resources from disk
                AFTER_LOAD,
                PRE_INIT,
                INIT, // init all the resources
                AFTER_INIT,
                PRE_UPLOAD,
                UPLOAD, // copying them onto API specific buffers and such
                AFTER_UPLOAD,
                HOT_LOAD, // do something right fucking now
                Stage_MAX 
            };

            Task();
            Task(Stage stage, uint64_t id, std::function<void(std::shared_ptr<uint8_t>)> func, std::shared_ptr<uint8_t> dat);

            void setOnCompleteCallback(std::function<void(Task task)> callback);

            bool isComplete();
            Stage getStage();
            std::shared_ptr<uint8_t> getDat();
        protected:
            std::future<void> execute(std::function<void()> finished);
            std::function<void(std::shared_ptr<uint8_t>)> func;
        private:
            std::shared_ptr<uint8_t> dat; // NO TEMPLATES
            Stage stage;
            uint64_t id;

            std::function<void(Task task)> callback;

            static void funcWrapper(std::function<void(std::shared_ptr<uint8_t>)> func, std::shared_ptr<uint8_t> dat,
             std::function<void()> finished, Task task,  std::function<void(Task task)> callback, std::shared_ptr<std::atomic_bool> completed);

            std::shared_ptr<std::atomic_bool> completed;

    };
    
}