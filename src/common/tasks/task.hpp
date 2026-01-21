#pragma once

#include <functional>
#include <string>
#include <future>

#include <logging/logger.hpp>

namespace Sierra {
    class Task {
        public:
            friend class TaskManager;

            Task();
            Task(std::string name, std::vector<std::string> dependencies, std::function<void(std::shared_ptr<uint8_t>)> func, std::shared_ptr<uint8_t> dat);

            void setOnCompleteCallback(std::function<void(Task task)> callback);


            std::string getName();
            std::vector<std::string>& getDependencies();

            bool isComplete();
            std::shared_ptr<uint8_t> getDat();
        protected:
            std::future<void> execute(std::function<void(Task& task)> finished);
            std::function<void(std::shared_ptr<uint8_t>)> func;

            bool markCompleteDependency();
        private:
            std::shared_ptr<uint8_t> dat; // NO TEMPLATES

            std::string name;
            std::vector<std::string> dependencies;
            uint32_t remainingDependencies;

            std::function<void(Task task)> callback;

            static void funcWrapper(std::function<void(std::shared_ptr<uint8_t>)> func, std::shared_ptr<uint8_t> dat,
             std::function<void(Task& task)> finished, Task& task,  std::function<void(Task task)> callback, std::shared_ptr<std::atomic_bool> completed);

            std::shared_ptr<std::atomic_bool> completed;

    };
    
}