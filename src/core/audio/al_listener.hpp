#pragma once

#include <glm/glm.hpp>
#include <AL/al.h>

namespace Sierra {
    //Abstraction for the OpenAL listener. It has methods for setting the position of the listener in 3D space.
    class ALListener {
        public:
            static void setPosition(const glm::vec3 &pos);
            static void setVelocity(const glm::vec3 &vel);
            static void setOrientation(const glm::vec3 &at, const glm::vec3 &up);
    };
}