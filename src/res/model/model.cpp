#include "model.hpp"

namespace Sierra {

    Assimp::Importer Model::importer = {};

    Model::Model() {

    }

    Model::Model(TaskManager& taskManager, std::string path): meshes() {
        loadScene(importer, path); //TODO move into task
    }


    void Model::loadScene(Assimp::Importer& importer, std::string path) {
        const aiScene* scene = importer.ReadFile(path,
             aiProcess_Triangulate | /*aiProcess_JoinIdenticalVertices | aiProcess_OptimizeMeshes |*/ aiProcess_OptimizeMeshes);


        if(!scene || !scene->mRootNode) {
            std::string err = "Failed to load model with path: " + path;  
            throw std::runtime_error(err);
        }

        for(int i = 0; i < scene->mNumMeshes; i++) {
            meshes.push_back(
                scene->mMeshes[i]
            );
        }
    }
}