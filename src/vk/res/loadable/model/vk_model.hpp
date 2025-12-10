#pragma once

#include "res/model/model.hpp"
#include "vk/res/mem/mem_loader.hpp"
#include "vk/res/loadable/texture/vk_texture.hpp"
#include "vk/res/mem/image/sampler.hpp"

namespace Sierra::vlk {
    class VlkModel {

        typedef std::array<std::vector<VlkTexture>, AI_TEXTURE_TYPE_COUNT> Textures_t;

        struct AsyncDat {
            size_t vertexCount;
            size_t indexCount;

            std::future<Buffer> vertexBuffFuture;
            std::future<Buffer> indexBuffFuture;

            Textures_t textures;

            std::shared_ptr<Model::AIModelData> modelData;
            std::string modelPath;

            Context* context;
            TaskManager* taskManager;
            MemoryManager* memManager;
            MemLoader* memLoader;

            VlkModel* parent;
            
            std::atomic_bool finished;
        };

        public:

            VlkModel();
            VlkModel(Context& context, TaskManager& taskManager, MemoryManager& memManager, MemLoader& memLoader, Model& model);

            VlkModel(VlkModel&) = delete;
            void operator=(VlkModel&) = delete;

            VlkModel(VlkModel&&);
            void operator=(VlkModel&&);

            Buffer& getVertexBuffer();
            size_t getVertexCount();

            Buffer& getIndexBuffer();
            size_t getIndexCount();

            VlkTexture* getTexture(aiTextureType type);

        private:
            static void createVertexBuff(AsyncDat& asyncDat);
            static void createIndexBuff(AsyncDat& asyncDat);
            static void createTextures(AsyncDat& asyncDat);

            void createTask(Context& context, TaskManager& taskManager, MemoryManager& memManager, MemLoader& memLoader, Model& model);
            static void createBuffers(std::shared_ptr<uint8_t> dat);
            static void finishedCallback(Task task);

            std::shared_ptr<AsyncDat> asyncDat;

            Buffer vertexBuff;
            std::future<Buffer> vertexBuffFuture;

            Buffer indexBuff;
            std::future<Buffer> indexBuffFuture;

            size_t vertexCount;
            size_t indexCount;

            Textures_t textures;

            Model model;

    };

}