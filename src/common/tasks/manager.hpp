#pragma once

#include <vector>
#include <array>
#include <utility>
#include <atomic>
#ifdef __linux__
#include <execinfo.h>
#endif
#include <cxxabi.h>

#include "task.hpp"
#include "util/utilities.hpp"

namespace Sierra {
    class TaskManager {
        friend class Task;

        typedef std::vector<std::string> TaskDeps;
        typedef std::unordered_map<std::string, std::pair<Task, TaskDeps>> TaskList;
        public:

            TaskManager();

            TaskManager(TaskManager&) = delete;
            TaskManager(TaskManager&&) = delete;

            void addTask(Task task);
            void addTasks(std::vector<Task>&& tasks);

            void printTasks();

            void start();
            bool isFinished();

        protected:
            void taskFinishedCallback(Task& task);
        private:
            void finish();

            TaskList::iterator findTask(std::string name);

             //to_underlying gets the enum number thingy

            TaskList tasks;
            TaskList startTasks;

            uint32_t tasksCompleted;

            std::atomic_bool finished;
    };
}