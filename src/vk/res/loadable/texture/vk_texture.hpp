#pragma once

#include <cstdint>
#include <future>

#include "vk/res/mem/mem_loader.hpp"
#include "res/texture/texture.hpp"

namespace Sierra::vlk {
    class VlkTexture {
        struct AsyncDat {
            std::shared_ptr<std::future<Image>> imageFuture;

            Texture* texture; //TODO handle moving

            Context* context;
            MemoryManager* manager;
            MemLoader* loader;
        };

        public:
            struct Info {
                Texture& texture;
            };

            VlkTexture();
            VlkTexture(Context& context, TaskManager& taskManager, MemoryManager& memManager, MemLoader& loader, Info info);

            VlkTexture(VlkTexture&) = delete;
            void operator=(VlkTexture&) = delete;

            VlkTexture(VlkTexture&&) = default;
            void operator=(VlkTexture&&);

            VkImage getImageHandle();

        private:
            void createTask(Context& context, TaskManager& taskManager, MemoryManager& memManager, MemLoader& loader, Info info);

            static void uploadToImage(std::shared_ptr<uint8_t> asyncDat);

            std::shared_ptr<AsyncDat> asyncDat;

            Image image;
    };
}