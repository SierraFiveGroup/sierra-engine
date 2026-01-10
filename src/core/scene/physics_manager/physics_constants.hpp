#pragma once

#include <glm/glm.hpp>
#include "util/utilities.hpp"

namespace Sierra{
    struct PhysicsConstants {
        glm::vec3 g = glm::vec3(0.0f, -9.18f, 0.0f); //This might a ticking time bomb - DON'T MULTIPLY BY THIS VECTOR :pray: :sob:
        float airDrag = 0.2f; 
        int c = 297792548;

        bool top_down_physics = false;
    };
}