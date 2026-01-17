#pragma once


#include "scene/component/component.hpp"
#include "logging/logger.hpp"

#include <glm/glm.hpp>

SIERRA_CORE_COMP(Transform3D)
    public:
        struct Data {
            glm::mediump_vec3 pos;
            glm::mediump_vec3 rotation;
            glm::mediump_vec3 scale;
        };

        Transform3D();

        void update() override;

        void setPos(glm::vec3 vec);
        void setRotation(glm::vec3 vec);
        void setScale(glm::vec3 vec);
    protected:
        struct Data data;
    //private: //the ONLY 3 members allowed to be here
        
END_SIERRA_COMP