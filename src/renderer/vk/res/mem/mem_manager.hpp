#pragma once

#include <vector>
#include <mutex>

#include <vulkan/vulkan.hpp>

#include "res/mem/buffer/buffer.hpp"
#include "res/command_buffer/command_buffer.hpp"

#include "core/context.hpp"
#include "sync/fence/fence.hpp"

#include "tasks/task.hpp"

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

            struct TransitionLayoutOp {
                VkImage image;

                VkImageLayout oldLayout;
                VkImageLayout newLayout;

                VkImageSubresourceRange range;

                std::function<void(TransitionLayoutOp)> callback;
                void* userDat;
            };

            MemoryManager();
            MemoryManager(Context& context);

            MemoryManager(MemoryManager&) = delete;

            MemoryManager(MemoryManager&&); 
            void operator=(MemoryManager&&);


            void addTransferOp(TransferOp op);
            void addTransitionLayoutOp(TransitionLayoutOp op);

            std::vector<Task> getTasks();
        protected:
        private:
            struct AsyncTransferDat {
                Context* context;

                std::shared_ptr<std::mutex> mutex;

                std::vector<TransferOp>* transferOps;
                CommandBuffer* cmdBuf;
            };

            struct AsyncTransitionDat {
                Context* context;

                std::shared_ptr<std::mutex> mutex;

                std::vector<TransitionLayoutOp>* transitionOps;
                CommandBuffer* cmdBuf;
            };

            static void asyncTransfer(std::shared_ptr<uint8_t> asyncDat);
            static void asyncTransition(std::shared_ptr<uint8_t> asyncDat);

            std::vector<TransferOp> transferOps;
            std::vector<TransitionLayoutOp> transitionOps;

            CommandPool cmdPool;
            CommandBuffer transferCmdBuf;
            CommandBuffer transitionCmdBuf;

            std::shared_ptr<AsyncTransferDat> transferDat;
            std::shared_ptr<AsyncTransitionDat> transitionDat;
            std::shared_ptr<std::mutex> transferDatMutex;
            std::shared_ptr<std::mutex> transitionDatMutex;

            Task transferTask;
            Task transitionTask;
            
            Context* context;
    };
}