#include "task.hpp"

namespace Sierra {
    Task::Task(): dat(), func(), id(), completed(), stage(Stage::Stage_MAX) {

    }
    
    Task::Task(Stage stage, uint64_t id, std::function<void(std::shared_ptr<uint8_t>)> func, std::shared_ptr<uint8_t> dat):
     func(func), dat(dat), stage(stage), id(id), completed(std::make_shared<std::atomic_bool>()) { 

    }

    std::future<void> Task::execute(std::function<void()> finished) {
        return std::async(&Task::funcWrapper, func, dat, finished, *this, callback, completed);
    }

    void Task::setOnCompleteCallback(std::function<void(Task task)> callback) {
        this->callback = callback;
    }

    void Task::funcWrapper(std::function<void(std::shared_ptr<uint8_t>)> func, std::shared_ptr<uint8_t> dat,
     std::function<void()> finished, Task task, std::function<void(Task task)> callback, std::shared_ptr<std::atomic_bool> completed) {
        try {
            func(dat);
        } catch(std::runtime_error e) {
            ERROR((std::string)"Error while running async task: " + e.what());
        }

        if(callback)
            callback(task);

        finished();

        *completed = true;
    }

    bool Task::isComplete() {
        return *completed;
    }

    std::shared_ptr<uint8_t> Task::getDat() {
        return dat; 
    }

    uint32_t Task::getID() {
        return id;
    }

    Task::Stage Task::getStage() {
        return stage;
    }
}