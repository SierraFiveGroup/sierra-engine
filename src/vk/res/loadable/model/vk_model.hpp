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

        private:
            void createVertexBuff(MemoryManager& memManager, MemLoader& memLoader);

            Buffer vertexBuff;
            std::future<Buffer> vertexBuffFuture;
            size_t vertexCount;

            Model model;
    };

}