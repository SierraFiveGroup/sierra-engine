#include "vk_texture.hpp"

namespace Sierra::vlk {

    VlkTexture::VlkTexture(): image(), asyncDat() {

    }

    VlkTexture::VlkTexture(Context& context, TaskManager& taskManager, MemoryManager& memManager, MemLoader& loader, Info info):
     image(), asyncDat(std::make_shared<AsyncDat>()) {
        createTask(context, taskManager, memManager, loader, info);
    }

    void VlkTexture::createTask(Context& context, TaskManager& taskManager, MemoryManager& memManager, MemLoader& loader, Info info) {
        asyncDat->imageFuture = std::make_shared<std::future<Image>>();
        asyncDat->context = &context;
        asyncDat->manager = &memManager;
        asyncDat->loader = &loader;
        asyncDat->texture = &info.texture;

        Task task = Task(Task::Stage::AFTER_LOAD, 0, uploadToImage, std::reinterpret_pointer_cast<uint8_t>(asyncDat));

        taskManager.addTask(task);
    }

    void VlkTexture::uploadToImage(std::shared_ptr<uint8_t> asyncDat) {
        AsyncDat& asyncDatRef = *std::reinterpret_pointer_cast<AsyncDat>(asyncDat).get();

        MemLoader::ImageInfo info{};
        switch(asyncDatRef.texture->getChannels()) {
            case 1:
                info.format = VK_FORMAT_R8_SRGB;
                break;
            case 4:
                info.format = VK_FORMAT_R8G8B8A8_SRGB;
                break;
            default:
                throw std::runtime_error("Error when trying to create the vulkan image, unknown channel count "
                     + std::to_string(asyncDatRef.texture->getChannels()));
        }
        info.extent = {(uint32_t)asyncDatRef.texture->getWidth(), (uint32_t)asyncDatRef.texture->getHeight(), 1};
        info.layout = VK_IMAGE_LAYOUT_GENERAL; // TODO WHATS RIGHT HERE
        info.usage = VK_IMAGE_USAGE_SAMPLED_BIT; // color attachment?

        *asyncDatRef.imageFuture = asyncDatRef.loader->createImage(
            *asyncDatRef.manager, info, asyncDatRef.texture->getDat(), info.extent.width * info.extent.height * asyncDatRef.texture->getChannels()
        );
    }

    VkImage VlkTexture::getImageHandle() {
        if (!image.getImage())
            image = asyncDat->imageFuture->get();

        return image.getImage();
    }

    void VlkTexture::operator=(VlkTexture&& other) {
        asyncDat = std::move(other.asyncDat);
        image = std::move(other.image);
    }
}