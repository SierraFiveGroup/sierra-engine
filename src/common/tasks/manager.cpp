#include "manager.hpp"

namespace Sierra {
    TaskManager::TaskManager(): tasks(), currentStage((Task::Stage)0), finished(true), tasksRemaining() {

    }

    void TaskManager::addTask(Task task) {
        tasks[ENUM_INT(task.getStage())].push_back(task);
    }

    void TaskManager::addTasks(std::vector<Task>&& tasks) {
        for(Task& task : tasks) {
            addTask(task);
        }
    }

    void TaskManager::start() { // TODO handle starting mid execution
        tasksRemaining = tasks[ENUM_INT(currentStage)].size();
        finished = false;

        if (!tasksRemaining) {
            advanceStage();
            return;
        }

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
        
        if(currentStage == Task::Stage::HOT_LOAD && 
            tasks[ENUM_INT(currentStage)].empty()) {
                finish();
                return;
        }
        
        tasksRemaining = tasks[ENUM_INT(currentStage)].size();

        if(!tasksRemaining) {
            advanceStage();
            return;
        }

        for (Task& task : tasks[ENUM_INT(currentStage)]) {
            task.execute(std::bind(&TaskManager::taskFinishedCallback, this));
        }
    }

#ifdef __linux__
    void TaskManager::printTasks() {
        LOG("Task Manager state:");
        for(int i = 0; i <   (int)Task::Stage::Stage_MAX; i++) {
            if(tasks[i].empty()) continue;
            LOG_NO_PRETTY(i << ":");
            for(Task task : tasks[i]) {
                auto methodPtr = task.func.target<void(*)(std::shared_ptr<uint8_t>)>();

                char** realname = backtrace_symbols((void * const*)methodPtr, 1); //fuck binbows
                //also for SOME REASOM target returns (**func) instead of (*func) so we gotta keep it
                std::string realNameStd = *realname;

                size_t openBracketPos = realNameStd.find_first_of('(');
                realNameStd = realNameStd.substr(openBracketPos + 1, realNameStd.find(')', openBracketPos + 1) - (openBracketPos + 1) - 2); // -2 because "+0" at the end fucks up the demangling
                //also the mangled name is in the brackets so we just get that

                char* realnameDemangled = abi::__cxa_demangle(realNameStd.c_str(), NULL, NULL, NULL);
                //bullshit fucking system
                //it feels like I have to summon cthulu himself just to get a damn name
                LOG_NO_PRETTY("\t" << realnameDemangled);
                free(realname);
                free(realnameDemangled);
            }
        }
    }
#else 
    void TaskManager::printTasks() {
        throw std::runtime_error("Task manager task debugging only supported on Linux, smd");
    }
#endif

};