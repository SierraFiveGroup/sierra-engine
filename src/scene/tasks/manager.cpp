#include "manager.hpp"

namespace Sierra {
    TaskManager::TaskManager(): tasks(), currentStage(Task::Stage::LOAD), finished(true), tasksRemaining() {

    }

    void TaskManager::addTask(Task task) {
        tasks[ENUM_INT(task.getStage())].push_back(task);
    }

    void TaskManager::start() { // TODO handle starting mid execution
        tasksRemaining = tasks[ENUM_INT(Task::Stage::LOAD)].size();
        currentStage = Task::Stage::LOAD;
        finished = false;

        for (Task& task : tasks[ENUM_INT(currentStage)]) {
            task.execute(std::bind(&TaskManager::taskFinishedCallback, this));
        }
    }

    bool TaskManager::isFinished() {
        return finished;
    }

    void TaskManager::taskFinishedCallback() {
        tasksRemaining--;

        if (!tasksRemaining) {
            advanceStage();
        }
    }

    void TaskManager::finish() {
        for(std::vector<Task>& taskBatch : tasks) {
            taskBatch.clear();
        }

        finished = true;
    }

    void TaskManager::advanceStage() {
        tasks[ENUM_INT(currentStage)].clear();

        currentStage = (Task::Stage)(ENUM_INT(currentStage) + 1);
        
        if(currentStage == Task::Stage::HOT_LOAD || 
            tasks[ENUM_INT(currentStage)].empty()) {
                finish();
                return;
        }
        
        tasksRemaining = tasks[ENUM_INT(currentStage)].size();

        for (Task& task : tasks[ENUM_INT(currentStage)]) {
            task.execute(std::bind(&TaskManager::taskFinishedCallback, this));
        }
    }
};