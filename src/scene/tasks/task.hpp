#pragma once

#include <functional>
#include <future>

namespace Sierra {
    class Task {
        public:
            friend class TaskManager;
            enum class Stage {
                LOAD, // loading resources from disk
                INIT, // init all the resources
                UPLOAD, // copying them onto API specific buffers and such
                HOT_LOAD, // do something right fucking now
                Stage_MAX = HOT_LOAD
            };

            Task();
            Task(Stage stage, std::function<void(std::unique_ptr<uint8_t>, std::function<void()>)> func, std::unique_ptr<uint8_t> dat);

            Task(Task&) = delete;

            Task(Task&&);
            void operator=(Task&&);

            
            bool isRunning();
            bool isComplete();
            Stage getStage();
        protected:
            std::future<void> execute(std::function<void()> finished);
        private:
            std::unique_ptr<uint8_t> dat; // NO TEMPLATES
            Stage stage;

            std::function<void(std::unique_ptr<uint8_t>, std::function<void()>)> func;

    };
    
}