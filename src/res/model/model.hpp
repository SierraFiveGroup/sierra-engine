#pragma once

#include "scene/tasks/manager.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/mesh.h>

namespace Sierra {
    namespace vlk {
        class VlkModel;
    }

    class Model {
        friend class Sierra::vlk::VlkModel;
        public:
            Model();
            Model(TaskManager& taskManager, std::string path);

        protected:

            std::vector<aiMesh*> meshes;
        private:
            void loadScene(Assimp::Importer& importer, std::string path);
            void processScene();

            //no point in creating our own mesh wrapper,
            //this doesn't complicate things

            static Assimp::Importer importer;
    };
}