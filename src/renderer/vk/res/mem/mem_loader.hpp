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

        struct OpPromise { //hellishly overcomplicated just for handling 2 options with 1 map
            std::optional<std::shared_ptr<std::promise<Buffer>>> bufferPromise;
            std::optional<std::shared_ptr<std::promise<Image>>> imagePromise;
        };

        typedef std::pair<OpRes, OpPromise> PromisePair_t;

        public:
            struct ImageInfo {
                VkFormat format;
                VkExtent3D extent;
                VkImageLayout layout;
                VkImageUsageFlagBits usage;
            };

            MemLoader(); // TODO rename to MemoryLoader for consistency
            MemLoader(Context& context);

            void clearAllocations();        

            MemLoader(MemLoader&) = delete;

            std::future<Buffer> createBuff(MemoryManager& manager, Buffer::Type type, Buffer::Usage usage, uint8_t* dat, size_t size);
            std::future<Image> createImage(MemoryManager& manager, ImageInfo info, uint8_t* dat, size_t size);
        private:
            Buffer createBuffHost(Buffer::Usage usage, uint8_t* dat, size_t size);
            std::future<Buffer> createBuffDevice(MemoryManager& manager, Buffer::Usage usage, uint8_t* dat, size_t size);

            std::vector<Buffer> stagingBuffers;
            std::unordered_map<size_t, PromisePair_t> transferPromises;

            std::shared_ptr<MemLoader*> selfPtr; // todo memory safety mutexing thingy

            Context* context;

            static void loadOpCallback(MemoryManager::TransferOp op);
    };
}