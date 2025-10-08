#pragma once

#include <vector>
#include <array>
#include <utility>
#include <atomic>

#include "task.hpp"
#include "util/utilities.hpp"

namespace Sierra {
    class TaskManager {
        friend class Task;
        public:

            TaskManager();

            TaskManager(TaskManager&) = delete;
            TaskManager(TaskManager&&) = delete;

            void addTask(Task task);
            void addTasks(std::vector<Task>&& tasks);

            void start();
            bool isFinished();
        protected:
            void taskFinishedCallback();
        private:
            void advanceStage();
            void finish();

            std::array<std::vector<Task>, ENUM_INT(Task::Stage::Stage_MAX)> tasks; //to_underlying gets the enum number thingy

            Task::Stage currentStage;
            std::atomic_uint32_t tasksRemaining;

            std::atomic_bool finished;
    };
}