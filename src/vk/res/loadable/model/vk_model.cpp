#include "vk_model.hpp"

#define VERTEX_SIZE (sizeof(float) * 3)

namespace Sierra::vlk {
    VlkModel::VlkModel(): model(), vertexCount(), indexCount() {

    }

    VlkModel::VlkModel(Context& context, TaskManager& taskManager, MemoryManager& memManager, MemLoader& memLoader, Model& model): model(model), vertexCount(), indexCount(), asyncDat(std::make_shared<AsyncDat>()) {
        createTask(context, taskManager, memManager, memLoader, model);
    }

    void VlkModel::createTask(Context& context, TaskManager& taskManager, MemoryManager& memManager, MemLoader& memLoader, Model& model) {
        asyncDat->parent = this;
        asyncDat->context = &context;
        asyncDat->memManager = &memManager;
        asyncDat->memLoader = &memLoader;
        asyncDat->taskManager = &taskManager;
        asyncDat->modelData = model.modelData;
        asyncDat->modelPath = model.getPath();

        Task task = Task(Task::Stage::LOAD, 0, createBuffers, std::reinterpret_pointer_cast<uint8_t>(asyncDat));
        task.setOnCompleteCallback(finishedCallback);

        taskManager.addTask(task);
    }

    void VlkModel::createBuffers(std::shared_ptr<uint8_t> dat) {
        AsyncDat& asyncDat = *(AsyncDat*)dat.get();

        if(asyncDat.modelData->meshes.empty()) throw std::runtime_error("Model not finished loading yet: " + asyncDat.modelPath);
        createVertexBuff(asyncDat);
        createIndexBuff(asyncDat);
        createTextures(asyncDat);
    }

    void VlkModel::createVertexBuff(AsyncDat& asyncDat) {

        if(asyncDat.modelData->meshes.size() == 1) {
            asyncDat.vertexCount = asyncDat.modelData->meshes[0]->mNumVertices;
            asyncDat.vertexBuffFuture = asyncDat.memLoader->createBuff(*asyncDat.memManager, Buffer::Type::DEVICE_LOCAL, 
                Buffer::Usage::VERTEX, (uint8_t*)asyncDat.modelData->meshes[0]->mVertices, asyncDat.modelData->meshes[0]->mNumVertices * VERTEX_SIZE);
             
            return; // so we dont create a whole new buffer
        } 

        asyncDat.vertexCount = 0;
        for(int i = 0; i < asyncDat.modelData->meshes.size(); i++) {
            asyncDat.vertexCount += asyncDat.modelData->meshes[i]->mNumVertices;
        }

        std::vector<float> buff(asyncDat.vertexCount * 3);

        size_t prevSize = 0;
        for(int i = 0; i < asyncDat.modelData->meshes.size(); i++) {
            memcpy(buff.data() + prevSize, asyncDat.modelData->meshes[i]->mVertices, asyncDat.modelData->meshes[i]->mNumVertices * VERTEX_SIZE);

            prevSize += asyncDat.modelData->meshes[i]->mNumVertices * sizeof(float);
        }

        asyncDat.vertexBuffFuture = asyncDat.memLoader->createBuff(*asyncDat.memManager, Buffer::Type::DEVICE_LOCAL, 
            Buffer::Usage::VERTEX, (uint8_t*)buff.data(), buff.size() * VERTEX_SIZE);
    }

    void VlkModel::createIndexBuff(AsyncDat& asyncDat) {
        for(aiMesh* mesh : asyncDat.modelData->meshes) {
            asyncDat.indexCount += mesh->mNumFaces * 3;
        }

        std::vector<uint32_t> indices(asyncDat.indexCount);

        int i = 0;
        for(aiMesh* mesh : asyncDat.modelData->meshes) {
            for(int y = 0; y < mesh->mNumFaces; y++) {
                indices[i + y*3 + 0] = mesh->mFaces[y].mIndices[0];
                indices[i + y*3 + 1] = mesh->mFaces[y].mIndices[1];
                indices[i + y*3 + 2] = mesh->mFaces[y].mIndices[2];
            }
        }

        asyncDat.indexBuffFuture = asyncDat.memLoader->createBuff(*asyncDat.memManager, Buffer::Type::DEVICE_LOCAL, Buffer::Usage::INDEX,
             (uint8_t*)indices.data(), indices.size() * sizeof(uint32_t));
    }

    void VlkModel::createTextures(AsyncDat& asyncDat) {
        Sampler::Info samplerInfo{};
        samplerInfo.filter = VK_FILTER_LINEAR; //TODO Option for linear/nearest
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR; //ditto
        samplerInfo.addressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.lodBias = 0.0;//??
        samplerInfo.minLod = 0.0;
        samplerInfo.maxLod = 1.0;
        samplerInfo.anisotropy = false; //TODO

        VlkTexture::Info texInfo{};
        texInfo.samplerInfo = samplerInfo;

        for(int i = 0; i < asyncDat.modelData->textures.size(); i++) {
            for(Texture& tex : asyncDat.modelData->textures[i]) {
                texInfo.texture = &tex;

                asyncDat.textures[i].push_back(
                    std::move(VlkTexture(*asyncDat.context, *asyncDat.taskManager, *asyncDat.memManager, *asyncDat.memLoader, texInfo))
                );
            }
        
        }
    }

    void VlkModel::finishedCallback(Task task) {
        AsyncDat& asyncDat = *(AsyncDat*)task.getDat().get();

        asyncDat.parent->vertexBuffFuture = std::move(asyncDat.vertexBuffFuture);
        asyncDat.parent->indexBuffFuture = std::move(asyncDat.indexBuffFuture);
        asyncDat.parent->vertexCount = asyncDat.vertexCount;
        asyncDat.parent->indexCount = asyncDat.indexCount;
        asyncDat.parent->textures = std::move(asyncDat.textures);

        asyncDat.finished = true;
    }
    
    size_t VlkModel::getVertexCount() {
        return vertexCount;
    }

    size_t VlkModel::getIndexCount() {
        return indexCount;
    }

    Buffer& VlkModel::getVertexBuffer() {
        if(vertexBuff.getBuff() == VK_NULL_HANDLE)
            vertexBuff = vertexBuffFuture.get();

        return vertexBuff;
    }

    Buffer& VlkModel::getIndexBuffer() {
        if(indexBuff.getBuff() == VK_NULL_HANDLE)
            indexBuff = indexBuffFuture.get();

        return indexBuff;
    }

    VlkTexture* VlkModel::getTexture(aiTextureType type) {
        if (!asyncDat->finished) 
            throw std::runtime_error("Tried to access model texture before loading is complete");

        if (textures[type].empty()) 
            return nullptr;

        return &textures[type].front();
        
    }

    VlkModel::VlkModel(VlkModel&& other) {
        if(!other.asyncDat) return;

        vertexBuff = std::move(other.vertexBuff);
        vertexBuffFuture = std::move(other.vertexBuffFuture);

        indexBuff = std::move(other.indexBuff);
        indexBuffFuture = std::move(other.indexBuffFuture);

        vertexCount = std::move(other.vertexCount);
        indexCount = std::move(other.indexCount);

        asyncDat = other.asyncDat;
        asyncDat->parent = this; // TODO handle concurrency cause this is hell
    }

    void VlkModel::operator=(VlkModel&& other) {
        if(!other.asyncDat) return;

        vertexBuff = std::move(other.vertexBuff);
        vertexBuffFuture = std::move(other.vertexBuffFuture);

        indexBuff = std::move(other.indexBuff);
        indexBuffFuture = std::move(other.indexBuffFuture);

        vertexCount = std::move(other.vertexCount);
        indexCount = std::move(other.indexCount);

        asyncDat = other.asyncDat;
        asyncDat->parent = this;
    }
}