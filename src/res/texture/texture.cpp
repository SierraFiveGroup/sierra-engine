#include "texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

namespace Sierra {

    Texture::Texture() {

    }

    Texture::Texture(TaskManager& manager, std::string path): asyncDat(std::make_shared<AsyncDat>()) {
        asyncDat->path = path;
        asyncDat->parent = this;

        addTask(manager);
    }

    void Texture::addTask(TaskManager& manager) {
        Task task = Task(Task::Stage::LOAD, 0, &Texture::loadTexture, std::reinterpret_pointer_cast<uint8_t>(asyncDat));
        task.setOnCompleteCallback(finishedCallback);
        manager.addTask(task);
    }

    void Texture::loadTexture(std::shared_ptr<uint8_t> asyncDat) {
        AsyncDat& asyncDatRef = *(AsyncDat*)asyncDat.get();

        asyncDatRef.data = stbi_load(asyncDatRef.path.c_str(), &asyncDatRef.x, &asyncDatRef.y, &asyncDatRef.channels, 0);
        if(!asyncDatRef.data) throw std::runtime_error((std::string)("Invalid image path ") + asyncDatRef.path);
    }

    void Texture::finishedCallback(Task task) {
        std::lock_guard lock(((AsyncDat*)task.getDat().get())->moveMutex);
        AsyncDat& asyncDatRef = *(AsyncDat*)task.getDat().get();
        Texture& tex = *asyncDatRef.parent;

        tex.x = asyncDatRef.x;
        tex.y = asyncDatRef.y;
        tex.data = asyncDatRef.data;
        tex.channels = asyncDatRef.channels;
        tex.loaded = true;
    }

    u_char* Texture::getPtr() {
        if(!isLoaded()) return nullptr; // prevent unlikely race condition
        return data;
    }

    int Texture::getWidth() {
        if(!isLoaded()) return 0;
        return x;
    }

    int Texture::getHeight() {
        if(!isLoaded()) return 0;
        return y;
    }

    int Texture::getChannels() {
        if(!isLoaded()) return 0;
        return channels;
    }

    bool Texture::isLoaded() {
        return loaded;
    }

    Texture::Texture(Texture&& other) {
        if(!other.asyncDat) return;

        std::lock_guard lock(other.asyncDat->moveMutex);

        asyncDat = other.asyncDat;
        x = other.x;
        y = other.y;
        channels = other.channels;
        data = other.data;
        asyncDat->parent = this;

        other.asyncDat = {};
        other.data = nullptr;
        other.x = 0;
        other.y = 0;
        other.channels = 0;
    }
    
    void Texture::operator=(Texture&& other) {
        if(!other.asyncDat) return;

        std::lock_guard lock(other.asyncDat->moveMutex);

        asyncDat = other.asyncDat;
        x = other.x;
        y = other.y;
        channels = other.channels;
        data = other.data;
        asyncDat->parent = this;

        other.asyncDat = {};
        other.data = nullptr;
        other.x = 0;
        other.y = 0;
        other.channels = 0;
    }
}