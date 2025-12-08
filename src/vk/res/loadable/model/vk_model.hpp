#pragma once

#include "res/model/model.hpp"
#include "vk/res/mem/mem_loader.hpp"

namespace Sierra::vlk {
    class VlkModel {
        struct AsyncDat {
            size_t vertexCount;
            size_t indexCount;

            std::future<Buffer> vertexBuffFuture;
            std::future<Buffer> indexBuffFuture;

            std::shared_ptr<Model::AIModelData> modelData;
            std::string modelPath;

            MemoryManager* memManager;
            MemLoader* memLoader;

            VlkModel* parent;
        };

        public:

            VlkModel();
            VlkModel(TaskManager& taskManager, MemoryManager& memManager, MemLoader& memLoader, Model& model);

            VlkModel(VlkModel&&);
            void operator=(VlkModel&&);

            Buffer& getVertexBuffer();
            size_t getVertexCount();

            Buffer& getIndexBuffer();
            size_t getIndexCount();

        private:
            static void createVertexBuff(AsyncDat& asyncDat);
            static void createIndexBuff(AsyncDat& asyncDat);

            void createTask(TaskManager& taskManager, MemoryManager& memManager, MemLoader& memLoader, Model& model);
            static void createBuffers(std::shared_ptr<uint8_t> dat);
            static void finishedCallback(Task task);

            std::shared_ptr<AsyncDat> asyncDat;

            Buffer vertexBuff;
            std::future<Buffer> vertexBuffFuture;

            Buffer indexBuff;
            std::future<Buffer> indexBuffFuture;

            size_t vertexCount;
            size_t indexCount;

            Model model;
    };

}