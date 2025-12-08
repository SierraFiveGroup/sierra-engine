#pragma once

#include <unordered_map>

#include "scene/tasks/manager.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/mesh.h>

#include "res/texture/texture.hpp"

namespace Sierra {
    namespace vlk {
        class VlkModel;
    }
        

    class Model {
        friend class Sierra::vlk::VlkModel;

        typedef std::array<std::vector<Texture>, 28/*number of material types*/> Textures_t;

        struct AIModelData { //packed into 1 struct cause fragmentation makes me sad
            Textures_t textures;
            std::vector<aiMesh*> meshes;
        };

        struct AsyncDat {
            TaskManager* taskManager;
            Assimp::Importer* importer;
            std::string modelPath;
            std::shared_ptr<std::atomic_bool> hasFinishedLoading;

            std::shared_ptr<AIModelData> modelData;
        };


        public:
            Model();
            Model(TaskManager& taskManager, std::string path);

            bool finishedLoading();
            std::string getPath();
        protected:
            std::shared_ptr<AIModelData> modelData;
        private:
            void createTask(TaskManager& taskManager);

            static void loadScene(std::shared_ptr<uint8_t> dat);
            static void getTextures(AsyncDat& asyncDat, const aiScene* scene);
            static void finishedCallback(Task task);

            //no point in creating our own mesh wrapper,
            //this doesn't complicate things

            static Assimp::Importer importer;
            std::string path;

        
            std::shared_ptr<AsyncDat> asyncDat;
            std::shared_ptr<std::atomic_bool> hasFinishedLoading;
    };
}