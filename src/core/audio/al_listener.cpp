#include "al_listener.hpp"

namespace Sierra {
    void ALListener::setPosition(const glm::vec3 &pos) {
        alListener3f(AL_POSITION, pos.x, pos.y, pos.z);
    }

    void ALListener::setVelocity(const glm::vec3 &vel) {
        alListener3f(AL_VELOCITY, vel.x, vel.y, vel.z);
    }

    void ALListener::setOrientation(const glm::vec3 &at, const glm::vec3 &up) {
        float orientation[6] = { at.x, at.y, at.z, up.x, up.y, up.z };
        alListenerfv(AL_ORIENTATION, orientation);
    }
}