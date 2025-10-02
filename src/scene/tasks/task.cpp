#include "task.hpp"

namespace Sierra {
    Task::Task(): dat(), func(), stage(Stage::Stage_MAX) {

    }
    
    Task::Task(Stage stage, std::function<void(std::unique_ptr<uint8_t>, std::function<void()>)> func, std::unique_ptr<uint8_t> dat):
     func(func), dat(dat.release()), stage(stage) { 

    }

    std::future<void> Task::execute(std::function<void()> finished) {
        return std::async(func, std::unique_ptr<uint8_t>(dat.release()), finished);
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


    Task::Task(Task&& other) {
        dat = std::unique_ptr<uint8_t>(other.dat.release());
        stage = other.stage;
        func = other.func;

        other.func = {0};
        other.stage = Stage::Stage_MAX;
    }

    void Task::operator=(Task&& other) {
        dat = std::unique_ptr<uint8_t>(other.dat.release());
        stage = other.stage;
        func = other.func;

        other.func = {0};
        other.stage = Stage::Stage_MAX;
    }
}