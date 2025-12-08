#include "model.hpp"

namespace Sierra {

    Assimp::Importer Model::importer = {};

    Model::Model() {

    }

    Model::Model(TaskManager& taskManager, std::string path): modelData(), path(path), hasFinishedLoading(std::make_shared<std::atomic_bool>()), asyncDat(std::make_shared<AsyncDat>()) {
        createTask(taskManager);
    }

    void Model::createTask(TaskManager& taskManager) {
        Task task = Task(Task::Stage::PRE_LOAD, 0, Model::loadScene, std::reinterpret_pointer_cast<uint8_t>(asyncDat));
        task.setOnCompleteCallback(finishedCallback);

        taskManager.addTask(task);

        asyncDat->taskManager = &taskManager;
        asyncDat->importer = &importer;
        asyncDat->modelPath = path;
        asyncDat->modelData = std::make_shared<AIModelData>();
        asyncDat->hasFinishedLoading = hasFinishedLoading;

        modelData = asyncDat->modelData;
    }


    void Model::loadScene(std::shared_ptr<uint8_t> dat) {
        AsyncDat& asyncDat = *(AsyncDat*)dat.get();

        const aiScene* scene = importer.ReadFile(asyncDat.modelPath,
             aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_OptimizeMeshes | aiProcess_OptimizeMeshes);


        if(!scene || !scene->mRootNode) {
            std::string err = "Failed to load model with path: " + asyncDat.modelPath;  
            throw std::runtime_error(err);
        }

        for(int i = 0; i < scene->mNumMeshes; i++) {
            asyncDat.modelData->meshes.push_back(
                scene->mMeshes[i]
            );
        }

        getTextures(asyncDat, scene);
    }

    void Model::getTextures(AsyncDat& asyncDat, const aiScene* scene) {
        for(int i = 0; i < scene->mNumMaterials; i++) {
            uint32_t texCount = scene->mMaterials[i]->GetTextureCount(aiTextureType_DIFFUSE);

            if(!texCount){
                continue;
            } 

            aiString texPath = {};
            scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &texPath);

            asyncDat.modelData->textures[aiTextureType_DIFFUSE].push_back(std::move(Texture(*asyncDat.taskManager, asyncDat.modelPath.substr(0, asyncDat.modelPath.find_last_of("/") + 1) + texPath.C_Str())));
        }
    }
    
    std::string Model::getPath() {
        return path;
    }

    void Model::finishedCallback(Task task) {
        AsyncDat& asyncDat = *(AsyncDat*)task.getDat().get();
        *asyncDat.hasFinishedLoading = true;
    }

    bool Model::finishedLoading() {
        if(!hasFinishedLoading) return false;
        return *hasFinishedLoading;
    }
}