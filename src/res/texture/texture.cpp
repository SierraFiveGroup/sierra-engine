#include "texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

namespace Sierra {

    Texture::Texture(): x(), y(), channels(), data() {

    }

    Texture::Texture(TaskManager& manager, std::string path): asyncDat(std::make_shared<AsyncDat>()), x(), y(), channels(), data() {
        asyncDat->path = path;
        asyncDat->parent = this;

        getImageInfo(path);
        addTask(manager);
    }

    Texture::~Texture() {
        if (asyncDat->data) {
            stbi_image_free(asyncDat->data);
        }
    }

    void Texture::getImageInfo(std::string path) {
        stbi_info(path.c_str(), &x, &y, &channels);

        if(!x || !y || !channels)
            throw std::runtime_error((std::string)("Invalid image path ") + path);

        if(channels > 1)
            channels = 4; //loadtexture for explanation
        asyncDat->channels = channels;
    }

    void Texture::addTask(TaskManager& manager) {
        Task task = Task(Task::Stage::LOAD, 0, &Texture::loadTexture, std::reinterpret_pointer_cast<uint8_t>(asyncDat));
        task.setOnCompleteCallback(finishedCallback);
        manager.addTask(task);
    }

    void Texture::loadTexture(std::shared_ptr<uint8_t> asyncDat) {
        AsyncDat& asyncDatRef = *(AsyncDat*)asyncDat.get();

        int req_comp = 0;
        if(asyncDatRef.channels > 1)
            req_comp = STBI_rgb_alpha; // cause of HORRIBLE format support the image has to be either red or rgba

        int channelsDummy;
        asyncDatRef.data = stbi_load(asyncDatRef.path.c_str(), &asyncDatRef.x, &asyncDatRef.y, &channelsDummy, req_comp);

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

    u_char* Texture::getDat() {
        if(!isLoaded()) return nullptr; // prevent unlikely race condition
        return data;
    }

    int Texture::getWidth() {
        return x;
    }

    int Texture::getHeight() {
        return y;
    }

    int Texture::getChannels() {
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