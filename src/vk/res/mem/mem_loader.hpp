#pragma once

#include "mem_manager.hpp"

#include <unordered_map>

namespace Sierra::vlk {
    class MemLoader {
        struct BuffPair {
            Buffer stagingBuff;
            Buffer deviceBuff;
        };

        typedef std::pair<BuffPair, std::shared_ptr<std::promise<Buffer>>> PromisePair_t;

        public:
            MemLoader();
            MemLoader(Context& context);

            void clearAllocations();        

            MemLoader(MemLoader&) = delete;

            std::future<Buffer> memToBuff(MemoryManager* manager, Buffer::Type type, Buffer::Usage usage, uint8_t* dat, size_t size);
        private:
            Buffer memToBuffHost(Buffer::Usage usage, uint8_t* dat, size_t size);
            std::future<Buffer> memToBuffDevice(MemoryManager* manager, Buffer::Usage usage, uint8_t* dat, size_t size);

            std::vector<Buffer> stagingBuffers;
            std::unordered_map<VkBuffer, PromisePair_t> bufferPromises;

            std::shared_ptr<MemLoader*> selfPtr; // todo memory safety mutexing thingy

            Context* context;

            static void loadOpCallback(MemoryManager::TransferOp op);
    };
}