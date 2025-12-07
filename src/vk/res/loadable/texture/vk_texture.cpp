#include "vk_texture.hpp"

#define IMG_LAYOUT VK_IMAGE_LAYOUT_GENERAL // as a sub until i figure out what the fuck to put
namespace Sierra::vlk {

    VlkTexture::VlkTexture(): image(), asyncDat(), sampler(), descriptorInfo() {

    }

    VlkTexture::VlkTexture(Context& context, TaskManager& taskManager, MemoryManager& memManager, MemLoader& loader, Info info):
     image(), asyncDat(std::make_shared<AsyncDat>()), sampler(context, info.samplerInfo), descriptorInfo(std::make_shared<VkDescriptorImageInfo>()) {
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
        info.layout = IMG_LAYOUT; // TODO WHATS RIGHT HERE
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

    VkDescriptorImageInfo* VlkTexture::getDescriptorInfo() {
        if(!image.getImage())
            return nullptr;

        descriptorInfo->sampler = sampler.getSampler();
        descriptorInfo->imageView = image.getView();
        descriptorInfo->imageLayout = IMG_LAYOUT;

        return descriptorInfo.get();
    }

    void VlkTexture::operator=(VlkTexture&& other) {
        *this = std::move(other);
    }
}