#include "loadable_resource.hpp"

namespace Sierra {

    LoadableResource::LoadableResource(std::string resourcePath): dat() {
        dat = std::make_shared<AsyncDat>();
        dat->filePath = resourcePath;
        dat->contents = {};

        task = Task(Task::Stage::LOAD, &LoadableResource::asyncLoad, std::reinterpret_pointer_cast<uint8_t>(dat));
    }

    void LoadableResource::asyncLoad(std::shared_ptr<void> asyncDat, std::function<void()> finished) {
        AsyncDat& dat = *(AsyncDat*)asyncDat.get();

        std::ifstream stream(dat.filePath, std::ios::binary | std::ios::ate); 

        size_t fileSize = stream.tellg();
        stream.seekg(0);

        std::vector<uint8_t> fileDat(fileSize);
        stream.read((char*)fileDat.data(), fileSize);

        dat.contents.set_value(std::move(fileDat));

        finished();
    }

    std::future<std::vector<uint8_t>> LoadableResource::getDat() {
        return dat->contents.get_future();
    }

    Task LoadableResource::getTask() {
        return task;
    }
}