#include "task.hpp"

namespace Sierra {
    Task::Task(): dat(), func(), stage(Stage::Stage_MAX) {

    }
    
    Task::Task(Stage stage, std::function<void(std::shared_ptr<uint8_t>, std::function<void()>)> func, std::shared_ptr<uint8_t> dat):
     func(func), dat(dat), stage(stage) { 

    }

    std::future<void> Task::execute(std::function<void()> finished) {
        return std::async(func, dat, finished);
    }

    bool Task::isRunning() {
        throw new std::runtime_error("UNIMPLEMENTED");
    }

    bool Task::isComplete() {
        throw new std::runtime_error("UNIMPLEMENTED");
    }

    Task::Stage Task::getStage() {
        return stage;
    }
}