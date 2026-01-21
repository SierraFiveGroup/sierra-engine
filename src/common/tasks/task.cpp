#include "task.hpp"

namespace Sierra {
    Task::Task(): dat(), func(), completed(), name() {

    }
    
    Task::Task(std::string name, std::vector<std::string> dependencies, std::function<void(std::shared_ptr<uint8_t>)> func, std::shared_ptr<uint8_t> dat):
     func(func), dat(dat), name(name), dependencies(dependencies), remainingDependencies(dependencies.size()), completed(std::make_shared<std::atomic_bool>()) { 

    }

    std::future<void> Task::execute(std::function<void(Task& task)> finished) {
        return std::async(std::launch::async, &Task::funcWrapper, func, dat, finished, std::ref(*this), callback, completed);
    }

    void Task::setOnCompleteCallback(std::function<void(Task task)> callback) {
        this->callback = callback;
    }

    void Task::funcWrapper(std::function<void(std::shared_ptr<uint8_t>)> func, std::shared_ptr<uint8_t> dat,
     std::function<void(Task& task)> finished, Task& task, std::function<void(Task task)> callback, std::shared_ptr<std::atomic_bool> completed) {
        try {
            func(dat);
        } catch(std::runtime_error e) {
            ERROR((std::string)"Error while running async task: " + e.what());
        }

        if(callback)
            callback(task);

        finished(task);

        *completed = true;
    }

    bool Task::isComplete() {
        return *completed;
    }

    bool Task::markCompleteDependency() {
        return !(--remainingDependencies);
    }

    std::shared_ptr<uint8_t> Task::getDat() {
        return dat; 
    }

    std::string Task::getName() {
        return name;
    }

    std::vector<std::string>& Task::getDependencies() {
        return dependencies;
    }
}