#include "loader.hpp"

namespace Sierra::vlk {
    Loader::Loader(): memLoader(), memManager() {

    }

    Loader::Loader(Context& context): context(&context), memManager(context), memLoader(context, memManager) {

    }

    std::vector<Res::ResourceAny> Loader::load(ResourceManager::_RendererLoadPacket& packet) {
        std::vector<Res::ResourceAny> resources;
        resources.reserve(packet.loadInfos.size());

        for(ResourceManager::_RendererLoadInfo& info : packet.loadInfos) {
            switch(info.type) {
                case (Res::Type::MODEL):
                    resources.emplace_back(loadModel(info, packet.taskManager));
                    break;
                case (Res::Type::TEXTURE):
                    resources.emplace_back(loadTexture(info, packet.taskManager));
                    break;
                default:
                    throw std::runtime_error("Unsupported resource type");
            }
        }

        packet.taskManager->addTasks(memManager.getTasks());

        return resources;
    }

    Res::ResourceAny Loader::loadModel(ResourceManager::_RendererLoadInfo& info, TaskManager* taskManager) {
        Res::Model modelRes{};
        modelRes.extraDat.reset((uint8_t*)new VlkModel(*context, *taskManager, memLoader, *info.baseRes.model), VlkModel::deleter);
        modelRes.type = Res::Type::MODEL;
        modelRes.isLoaded = VlkModel::isLoaded;
        //modelRes.textures

        Res::ResourceAny resAny{};
        resAny.model = modelRes;

        return resAny;
    }

    Res::ResourceAny Loader::loadTexture(ResourceManager::_RendererLoadInfo& info, TaskManager* taskManager) {
        VlkTexture::Info texInfo{};
        texInfo.texture = info.baseRes.texture;
        texInfo.samplerInfo.addressMode =  VK_SAMPLER_ADDRESS_MODE_REPEAT;
        texInfo.samplerInfo.anisotropy = false;
        texInfo.samplerInfo.filter = VK_FILTER_LINEAR;
        texInfo.samplerInfo.lodBias = 0.0;
        texInfo.samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

        Res::Texture texRes{};
        texRes.extraDat.reset((uint8_t*)new VlkTexture(*context, *taskManager, memLoader, texInfo), VlkTexture::deleter);
        texRes.type = Res::Type::TEXTURE;
        texRes.isLoaded = VlkTexture::isLoaded;

        Res::ResourceAny resAny{};
        resAny.texture = texRes;

        return resAny;
    }
}