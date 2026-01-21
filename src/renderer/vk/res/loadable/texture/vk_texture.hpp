#pragma once

#include <cstdint>
#include <future>

#include "res/mem/mem_loader.hpp"
#include "res/texture/texture.hpp"
#include "res/mem/image/sampler.hpp"
#include "resource_manager/resources/resources.hpp"

namespace Sierra::vlk {
    class VlkTexture {
        friend class Loader;

        struct AsyncDat {
            std::shared_ptr<MemLoader::ImageFuture_t> imageFuture;

            Texture* texture; //TODO handle moving

            Context* context;
            MemLoader* loader;
        };

        public:
            struct Info {
                Texture* texture;
                Sampler::Info samplerInfo;
            };

            VlkTexture();
            VlkTexture(Context& context, TaskManager& taskManager, MemLoader& loader, Info info);

            VlkTexture(VlkTexture&) = delete;
            void operator=(VlkTexture&) = delete;

            VlkTexture(VlkTexture&&) = default;
            void operator=(VlkTexture&&);

            VkImage getImageHandle();
            VkDescriptorImageInfo* getDescriptorInfo();

        protected:
            static bool isLoaded(Res::ResourceAny res);
            static void deleter(void* obj);

        private:
            void createTask(Context& context, TaskManager& taskManager, MemLoader& loader, Info info);

            static void uploadToImage(std::shared_ptr<uint8_t> asyncDat);

            std::shared_ptr<AsyncDat> asyncDat;

            Image image;
            Sampler sampler;
            std::shared_ptr<VkDescriptorImageInfo> descriptorInfo;
    };
}