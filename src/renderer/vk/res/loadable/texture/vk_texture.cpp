#include "vk_texture.hpp"

#define IMG_LAYOUT VK_IMAGE_LAYOUT_GENERAL // as a sub until i figure out what the fuck to put
namespace Sierra::vlk {

    VlkTexture::VlkTexture(): image(), asyncDat(), sampler(), descriptorInfo() {

    }

    VlkTexture::VlkTexture(Context& context, TaskManager& taskManager, MemLoader& memLoader, Info info):
     image(), asyncDat(std::make_shared<AsyncDat>()), sampler(context, info.samplerInfo), descriptorInfo(std::make_shared<VkDescriptorImageInfo>()) {
        createTask(context, taskManager, memLoader, info);
    }

    void VlkTexture::createTask(Context& context, TaskManager& taskManager, MemLoader& loader, Info info) {
        asyncDat->imageFuture = std::make_shared<MemLoader::ImageFuture_t>();
        asyncDat->context = &context;
        asyncDat->loader = &loader;
        asyncDat->texture = std::move(info.texture);

        Task task = Task(Task::Stage::PRE_INIT, 0, uploadToImage, std::reinterpret_pointer_cast<uint8_t>(asyncDat));

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
            info, asyncDatRef.texture->getDat(), info.extent.width * info.extent.height * asyncDatRef.texture->getChannels()
        );
    }

    VkImage VlkTexture::getImageHandle() {
        if (!image.getImage())
            image = asyncDat->imageFuture->get().first;

        return image.getImage();
    }

    VkDescriptorImageInfo* VlkTexture::getDescriptorInfo() {
        if(!image.getImage())
            image = asyncDat->imageFuture->get().first;

        descriptorInfo->sampler = sampler.getSampler();
        descriptorInfo->imageView = image.getView();
        descriptorInfo->imageLayout = IMG_LAYOUT;

        return descriptorInfo.get();
    }

    bool VlkTexture::isLoaded(Res::ResourceAny res) {
        VlkTexture& tex = *(VlkTexture*)res.base.extraDat.get();

        return tex.image.getImage() || tex.asyncDat->imageFuture->valid();
    }

    void VlkTexture::deleter(void* obj) {
        VlkTexture* tex = (VlkTexture*)obj;
        delete tex;
    }

    void VlkTexture::operator=(VlkTexture&& other) {
        asyncDat = std::move(other.asyncDat);
        image = std::move(other.image);
        sampler = std::move(other.sampler);
        descriptorInfo = std::move(other.descriptorInfo);
    }
}