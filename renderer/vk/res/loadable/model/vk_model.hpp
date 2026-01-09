#pragma once

#include "res/model/model.hpp"
#include "res/mem/mem_loader.hpp"
#include "res/loadable/texture/vk_texture.hpp"
#include "res/mem/image/sampler.hpp"

namespace Sierra::vlk {
    class VlkModel {

        typedef std::array<std::vector<VlkTexture>, AI_TEXTURE_TYPE_COUNT> Textures_t;

        public: struct Mesh;
        protected:

        struct AsyncDat {
            size_t vertexCount;
            size_t indexCount;

            std::future<Buffer> vertexBuffFuture;
            std::future<Buffer> indexBuffFuture;

            Textures_t textures;

            std::vector<Mesh> meshes;

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

            struct Mesh {
                size_t vertexBuffOffset;
                size_t texCoordBuffOffset;
                size_t vertexCount;
            };

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

            const std::vector<Mesh>& getMeshes();

        private:
            static void createVertexBuff(AsyncDat& asyncDat); // TODO, make configurable what you wanna load
            static void createIndexBuff(AsyncDat& asyncDat); //i.e. tex coords, normals, vertices
            static void createTextures(AsyncDat& asyncDat);

            void createTask(Context& context, TaskManager& taskManager, MemoryManager& memManager, MemLoader& memLoader, Model& model);
            static void createBuffers(std::shared_ptr<uint8_t> dat);
            static void finishedCallback(Task task);

            std::shared_ptr<AsyncDat> asyncDat;

            Buffer vertexBuff;
            std::future<Buffer> vertexBuffFuture;

            Buffer indexBuff;
            std::future<Buffer> indexBuffFuture;

            std::vector<Mesh> meshes;

            size_t vertexCount;
            size_t indexCount;

            Textures_t textures;

            Model model;

    };

}