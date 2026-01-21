#include "render_cache.hpp"

namespace Sierra::vlk {
    RenderCache::RenderCache() {

    }

    RenderCache::RenderCache(MemLoader& memLoader): memLoader(&memLoader) {

    }
    
    std::optional<RenderCache::BatchCache*> RenderCache::getRefreshedCache(Renderer::ObjectBatch& batch) {
        auto it = caches.find(batch.modelID);
        BatchCache& cache = it->second;

        if(!cache.instanceBufferFuture.valid()) { // nothing to wait on
            return {&cache};
        }

        std::future_status status = cache.instanceBufferFuture.wait_for(std::chrono::milliseconds(0)); // get it instantly

        if(status == std::future_status::timeout) {
            return std::nullopt;
        } 

        cache.instanceBuffer = std::move(cache.instanceBufferFuture.get());
        return {&cache};
    }

    std::unordered_map<Res::ResID, RenderCache::BatchCache>::iterator RenderCache::createCache(Renderer::ObjectBatch& batch) {
        auto it = caches.insert(caches.begin()/*so we get the it*/, std::make_pair(batch.modelID, (BatchCache){}));

       // it->second.instanceBufferFuture = memLoader->createBuff(Buffer::Type::DEVICE_LOCAL, Buffer::Usage::VERTEX, (uint8_t*)batch.instanceData.data(),
       //  batch.instanceData.size() * sizeof(Component::Transform3D::Data));

       //GLOBAL HIVE OF STATES FOR BUFFERS AND IMAGES -> STATE CAN BE SIGNALED BY MEM LOADER AND READ EASILY

        return it;
    }

}