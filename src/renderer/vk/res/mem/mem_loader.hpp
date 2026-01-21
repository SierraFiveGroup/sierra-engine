#pragma once

#include "mem_manager.hpp"
#include "image/image.hpp"

#include <unordered_map>
#include <optional>

namespace Sierra::vlk {
    class MemLoader {
        struct OpRes {
            Buffer stagingBuff;

            std::optional<Buffer> buffer;
            std::optional<Image> image;
        };

        public:
            typedef std::optional<Buffer> BuffOpt_t;
            typedef std::future<std::pair<Buffer, BuffOpt_t>> BufferFuture_t ;
            typedef std::future<std::pair<Image, BuffOpt_t>> ImageFuture_t;

        protected:
            typedef std::promise<std::pair<Buffer, BuffOpt_t>> BufferPromise_t ;
            typedef std::promise<std::pair<Image, BuffOpt_t>> ImagePromise_t;

        struct OpPromise { //hellishly overcomplicated just for handling 2 options with 1 map
            std::optional<BufferPromise_t> bufferPromise;
            std::optional<ImagePromise_t> imagePromise;
        };

        public:

            struct ImageInfo {
                VkFormat format;
                VkExtent3D extent;
                VkImageLayout layout;
                VkImageUsageFlagBits usage;
            };

            MemLoader(); // TODO rename to MemoryLoader for consistency
            MemLoader(Context& context, MemoryManager& memManager);

            void clearAllocations();        

            MemLoader(MemLoader&) = delete;
            MemLoader(MemLoader&&) = delete;

            BufferFuture_t createBuff(Buffer::Type type, Buffer::Usage usage, uint8_t* dat, size_t size);
            ImageFuture_t createImage(ImageInfo info, uint8_t* dat, size_t size);

            BufferFuture_t updateBuff(Buffer&& dst, Buffer& src, uint8_t* dat, size_t size);
        private:
            typedef std::pair<OpRes, OpPromise> PromisePair_t;
        
            Buffer createBuffHost(Buffer::Usage usage, uint8_t* dat, size_t size);
            std::future<std::pair<Buffer, BuffOpt_t>> createBuffDevice(MemoryManager& manager, Buffer::Usage usage, uint8_t* dat, size_t size);

            std::vector<Buffer> stagingBuffers;
            std::unordered_map<size_t, PromisePair_t> transferPromises;

            Context* context;
            MemoryManager* memManager;

            static void loadOpCallback(MemoryManager::TransferOp op);
    };
}