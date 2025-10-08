#pragma once

#include <vector>
#include <mutex>

#include <vulkan/vulkan.hpp>

#include "vk/res/mem/buffer/buffer.hpp"
#include "vk/res/command_buffer/command_buffer.hpp"

#include "vk/core/context.hpp"
#include "vk/sync/fence/fence.hpp"

#include "scene/tasks/task.hpp"

namespace Sierra::vlk {

    class MemoryManager {
        public:
            struct TransferOp {
                VkBuffer src;

                size_t size;

                size_t srcOffset;
                size_t dstOffset;

                VkImageSubresourceLayers subResLayers;
                VkExtent3D imageExtent;
                VkImageLayout imageLayout;
            
                VkBuffer buffer;
                VkImage image;

                std::function<void(TransferOp)> callback;
                void* userDat;
            };

            MemoryManager(Context& context);

            MemoryManager(MemoryManager&) = delete;

            MemoryManager(MemoryManager&&); 
            void operator=(MemoryManager&&);


            void addTransferOp(TransferOp op);

            Task getTask();
        protected:
        private:
            struct AsyncDat {
                Context* context;

                std::shared_ptr<std::mutex> mutex;

                std::vector<TransferOp>* transferOps;
                CommandBuffer* cmdBuf;
            };

            static void asyncTransfer(std::shared_ptr<uint8_t> asyncDat);

            std::vector<TransferOp> transferOps;

            CommandPool cmdPool;
            CommandBuffer cmdBuf;

            std::shared_ptr<AsyncDat> asyncDat;
            std::shared_ptr<std::mutex> asyncDatMutex;

            Task task;
            
            Context* context;
    };
}