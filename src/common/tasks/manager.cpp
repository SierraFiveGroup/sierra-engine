#include "manager.hpp"

namespace Sierra {
    TaskManager::TaskManager(): tasks(), finished(true) {

    }

    void TaskManager::addTask(Task task) {
        //if(!finished) throw new std::runtime_error("Tried to add task while taskManager is running");
        //we'll see if we dont need this and can just wing it

        if(task.getDependencies().empty()) {
            startTasks[task.getName()] = {task, {}};
            return;
        }

        tasks[task.getName()] = {task, {}};

        for(std::string dep : task.getDependencies()) {
                try {
                    TaskList::iterator it = findTask(dep);
                    it->second.second.push_back(task.getName());
                } catch(std::runtime_error e) {
                    throw std::runtime_error("Not all required task dependencies present: " + (std::string)e.what());
                }
                //kinda convoluted looking but it actually just adds the current task name to the list of the task it depends on
                //that task can then signal that the current task that its done
            }
        }

    void TaskManager::addTasks(std::vector<Task>&& tasks) {
        for(Task& task : tasks) {
            addTask(task);
        }
    }

    void TaskManager::start() { // TODO handle starting mid execution
        tasksCompleted = 0;

        for (auto& task : startTasks) {
            task.second.first.execute(std::bind(&TaskManager::taskFinishedCallback, this, std::placeholders::_1));
        }
    }

    bool TaskManager::isFinished() {
        return finished;
    }

    void TaskManager::taskFinishedCallback(Task& task) {
        auto it = findTask(task.getName());

        for(std::string dep : it->second.second) {
            Task& task = tasks[dep].first;
            if(!task.markCompleteDependency()) continue; //all dependencies have finished, we can safely start this

            task.execute(std::bind(&TaskManager::taskFinishedCallback, this, std::placeholders::_1));
        }

        tasksCompleted++;

        if(tasksCompleted == tasks.size() + startTasks.size()) finish();
    }

    TaskManager::TaskList::iterator TaskManager::findTask(std::string name) {
        auto it = startTasks.find(name);
        if(it != startTasks.end()) return it;

        it = tasks.find(name);
        if(it != tasks.end()) return it;

        throw std::runtime_error("Task not found");
    }

    void TaskManager::finish() {
        tasks.clear();
        startTasks.clear();

        finished = true;
    }

    void TaskManager::printTasks() {
        LOG("Task Manager state:");


        for(auto task : startTasks) {
            LOG_NO_PRETTY(task.second.first.getName() << ": NO DEPENDENCIES");
        }

        for(auto task : tasks) {
            LOG_NO_PRETTY(task.second.first.getName() << ":");
            for(std::string dep : task.second.second) {
                LOG_NO_PRETTY("\t" << dep);
            }
        }
    }


};