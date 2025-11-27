#include "vk_model.hpp"

#define VERTEX_SIZE (sizeof(float) * 3)

namespace Sierra::vlk {
    VlkModel::VlkModel(): model(), vertexCount() {

    }

    VlkModel::VlkModel(MemoryManager& memManager, MemLoader& memLoader, Model& model): model(model), vertexCount() {
        createVertexBuff(memManager, memLoader);
    }

    void VlkModel::createVertexBuff(MemoryManager& memManager, MemLoader& memLoader) {
        if(model.meshes.size() == 1) {
            vertexCount = model.meshes[0]->mNumVertices;
            vertexBuffFuture = memLoader.createBuff(memManager, Buffer::Type::DEVICE_LOCAL, 
                Buffer::Usage::VERTEX, (uint8_t*)model.meshes[0]->mVertices, model.meshes[0]->mNumVertices * VERTEX_SIZE);
             
            return; // so we dont create a whole new buffer
        } 

        vertexCount = 0;
        for(int i = 0; i < model.meshes.size(); i++) {
            vertexCount += model.meshes[i]->mNumVertices;
        }

        std::vector<float> buff(vertexCount * 3);

        size_t prevSize = 0;
        for(int i = 0; i < model.meshes.size(); i++) {
            memcpy(buff.data() + prevSize, model.meshes[i]->mVertices, model.meshes[i]->mNumVertices * VERTEX_SIZE);

            prevSize += model.meshes[i]->mNumVertices * sizeof(float);
        }

        vertexBuffFuture = memLoader.createBuff(memManager, Buffer::Type::DEVICE_LOCAL, 
            Buffer::Usage::VERTEX, (uint8_t*)buff.data(), buff.size() * VERTEX_SIZE);
    }
    
    size_t VlkModel::getVertexCount() {
        return vertexCount;
    }

    Buffer& VlkModel::getVertexBuffer() {
        if(vertexBuff.getBuff() == VK_NULL_HANDLE)
            vertexBuff = vertexBuffFuture.get();

        return vertexBuff;
    }
}