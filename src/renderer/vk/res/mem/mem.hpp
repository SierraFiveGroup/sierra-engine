#pragma once

#include "core/context.hpp"
#include "util.hpp"
#include "scene/ref/resource_ref.hpp"

#include "vk_mem_alloc.h"

#include <unordered_map>
#include <memory>


namespace Sierra::vlk {
    class Mem {
        friend class MemPool;
        friend class Buffer;
        friend class Image;

        public:
            enum class Type {
                AUTO,
                PREFER_DEVICE,
                PREFER_HOST,
            };

            struct Info {
                size_t flags;
                Type type;
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

            VkDeviceSize getSize();
            VkDeviceSize getOffset();
            
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