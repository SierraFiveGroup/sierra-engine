#pragma once

#include <string>
#include <atomic>
#include "io/logging/logger.hpp"
#include "scene/tasks/manager.hpp"

namespace Sierra {
    class Texture {
        struct AsyncDat {
            std::string path;
            u_char* data;
            int x, y, channels;
            std::mutex moveMutex;

            std::atomic<Texture*> parent;
        };

        public:
            Texture();
            ~Texture();
            Texture(TaskManager& manager, std::string path);

            Texture(Texture&) = delete;
            void operator=(Texture&) = delete;

            u_char* getDat();
            int getWidth();
            int getHeight();
            int getChannels();
            bool isLoaded();

            Texture(Texture&&);
            void operator=(Texture&&);
        private:
            void getImageInfo(std::string path);

            void addTask(TaskManager& manager);
            
            static void loadTexture(std::shared_ptr<uint8_t> asyncDat);
            static void finishedCallback(Task task);

            std::shared_ptr<AsyncDat> asyncDat;
            int x, y, channels;
            u_char* data;
            
            std::atomic_bool loaded;
    };
}