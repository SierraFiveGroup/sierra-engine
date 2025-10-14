#include "task.hpp"

namespace Sierra {
    Task::Task(): dat(), func(), id(), stage(Stage::Stage_MAX) {

    }
    
    Task::Task(Stage stage, uint64_t id, std::function<void(std::shared_ptr<uint8_t>)> func, std::shared_ptr<uint8_t> dat):
     func(func), dat(dat), stage(stage), id(id) { 

    }

    std::future<void> Task::execute(std::function<void()> finished) {
        return std::async(&Task::funcWrapper, func, dat, finished, *this, callback);
    }

    void Task::setOnCompleteCallback(std::function<void(Task task)> callback) {
        this->callback = callback;
    }

    void Task::funcWrapper(std::function<void(std::shared_ptr<uint8_t>)> func, std::shared_ptr<uint8_t> dat,
     std::function<void()> finished, Task task, std::function<void(Task task)> callback) {
        func(dat);
        finished();
        callback(task);
     }

    bool Task::isComplete() {
        throw std::runtime_error("UNIMPLEMENTED");
    }

    Task::Stage Task::getStage() {
        return stage;
    }
}