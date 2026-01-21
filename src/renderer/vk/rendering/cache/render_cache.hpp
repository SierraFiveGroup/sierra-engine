#pragma once

#include <unordered_map>
#include <optional>

#include "resource_manager/resources/resources.hpp"
#include "renderer/renderer.hpp"
#include "renderer/vk/res/mem/buffer/buffer.hpp"
#include "renderer/vk/res/mem/mem_loader.hpp"

namespace Sierra::vlk {
    class RenderCache {
        public:
            struct BatchCache {
                Buffer instanceBuffer;
                std::future<Buffer> instanceBufferFuture;
                VlkModel* model;
            };

            RenderCache();
            RenderCache(MemLoader& memLoader);

            std::optional<BatchCache*> getRefreshedCache(Renderer::ObjectBatch& batch);

        private:
            std::unordered_map<Res::ResID, BatchCache>::iterator createCache(Renderer::ObjectBatch& batch);

            std::unordered_map<Res::ResID, BatchCache> caches;

            MemLoader* memLoader;

    };
}