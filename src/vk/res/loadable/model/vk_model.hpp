#pragma once

#include "res/model/model.hpp"
#include "vk/res/mem/mem_loader.hpp"

namespace Sierra::vlk {
    class VlkModel {
        public:

            VlkModel();
            VlkModel(MemoryManager& memManager, MemLoader& memLoader, Model& model);

            Buffer& getVertexBuffer();
            size_t getVertexCount();

            Buffer& getIndexBuffer();
            size_t getIndexCount();

        private:
            void createVertexBuff(MemoryManager& memManager, MemLoader& memLoader);
            void createIndexBuff(MemoryManager& memManager, MemLoader& memLoader);

            Buffer vertexBuff;
            std::future<Buffer> vertexBuffFuture;

            Buffer indexBuff;
            std::future<Buffer> indexBuffFuture;

            size_t vertexCount;
            size_t indexCount;

            Model model;
    };

}