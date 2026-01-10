#pragma once

#include <glm/glm.hpp>

#include "input/input.hpp"

namespace Sierra {
    struct StateInfo {
        InputHandler& inputHandler;

        float dt;
    };
}