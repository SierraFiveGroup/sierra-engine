#pragma once

#include "vk/core/context.hpp"
#include "vk/util.hpp"
#include "scene/ref/resource_ref.hpp"

#include "vk/vk_mem_alloc.h"

#include <unordered_map>
#include <memory>


namespace Sierra::vlk {
    class Mem {
        friend class MemPool;
        friend class Buffer;
        friend class Image;

        public:
            static uint32_t ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
            static uint32_t ALLOCATION_CREATE_NEVER_ALLOCATE_BIT;
            static uint32_t ALLOCATION_CREATE_MAPPED_BIT;
            static uint32_t ALLOCATION_CREATE_WITHIN_BUDGET_BIT;
            static uint32_t ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
            static uint32_t ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;
            static uint32_t ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT;
            static uint32_t ALLOCATION_CREATE_STRATEGY_MIN_MEMORY_BIT;
            static uint32_t ALLOCATION_CREATE_STRATEGY_MIN_TIME_BIT;

            enum class Usage {
                AUTO,
                PREFER_DEVICE,
                PREFER_HOST,
            };

            struct Info {
                size_t flags;
                Usage usage;
                VkMemoryPropertyFlags required;
                VkMemoryPropertyFlags preferred;
                float priority;
            };

            /// STATIC PART
            static void init(Context& context);
            static void destroy(Context& context);
            /////////

            Mem();
            Mem(Context& context, Info& info, VkBufferCreateInfo& bufferInfo, VkBuffer& buffer);
            Mem(Context& context, Info& info, VkImageCreateInfo& imageInfo, VkImage& image);

            Mem(Mem&) = delete;
            Mem(Mem&&);
            void operator=(Mem&&);

            ~Mem();

            void* map();
            void unmap();
            void copyToHost(uint8_t* src, size_t srcSize);
            
        protected:
            VmaAllocationInfo& getAllocInfo();            
        private:
            /// STATIC PART
            static VmaAllocator createAllocator(Context& context);
            /////////

            void createBuffer(Info& info, VkBufferCreateInfo& bufferInfo, VkBuffer& buffer);
            void createImage(Info& info, VkImageCreateInfo& imageInfo, VkImage& image);

            VkBuffer buffer;
            VkImage image;

            VmaAllocation allocation;            
            ResourceRef<VmaAllocator> allocator;

            VmaAllocationInfo allocInfo;
    };

    class MemPool {
        //TODO this is simply in place if ever needed, for now I'll just use the pools that are alloc'd by default 
        // 2 in 1 file?? this shit is absolutely fucking me over
        //type of design that makes me want to saw my head clean off
        //yes I will bitch about it endlessly
        public:
            struct Info {
                uint32_t memTypeIndex;
                size_t minBlockCount;
                size_t maxBlockCount;
                float priority;
                VkDeviceSize minAlignment;
            };

            MemPool();
            MemPool(Context& context, Info& info);
        private:

            ResourceRef<VmaAllocator> allocator;
    };
}