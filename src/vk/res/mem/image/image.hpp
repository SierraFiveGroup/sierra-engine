#pragma once

#include <unordered_map>
#include <memory>

#include "../mem.hpp"
#include "../mem_manager.hpp"

namespace Sierra::vlk {
    class Image {
        typedef std::unordered_map<VkImage, std::shared_ptr<Image*>> ImageMap_t;

        public:
            struct Info {
                VkImageType imageType;
                VkFormat format;
                VkExtent3D extent;
                uint32_t mipLevels;
                VkImageTiling tiling;
                VkSampleCountFlagBits samples;
                std::vector<uint32_t> queueFamilyIndices;
                VkImageLayout layout; // todo automatically handle layout transitions??
                VkImageUsageFlags usage;

                Mem::Type type;
            };

            Image();
            Image(Context& context, MemoryManager& manager, Info info);

            Image(Image&) = delete;

            Image(Image&&);
            void operator=(Image&&);

            ~Image(); // remove from map

            VkImage getImage();
        private:
            void createImage(Context& context, Info& info);
            void addTransitionOp(MemoryManager& manager, Info& info);

            void completeTransition(); 

            static void transitionOpCallback(MemoryManager::TransitionLayoutOp op);

            static ImageMap_t imagePtrMap; //absolute dogshit design that will work
            //bascially this table is used for the op callback to find the correct image class, the image address should be updated when its moved and such

            std::shared_ptr<std::atomic_bool> available; //if transition/transfer is complete or something

            VkImage image;
            Mem mem;
    };
}