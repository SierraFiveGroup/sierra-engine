#pragma once

#include "util/utilities.hpp"
#include "al_buffer.hpp"
#include <AL/al.h>
#include <glm/glm.hpp>

namespace Sierra {
    //Handler for the OpenAL sounds. Loaded via ALBuffers.
    class ALSound {
        public:
            ALSound(ALBuffer &buf);
            DISABLE_COPY_AND_MOVE(ALSound);

            ~ALSound();

            void play();
            void pause();
            void stop();
            void setLooping(bool loop);
            void setGain(float gain);

            void setPosition(const glm::vec3 &pos);
            glm::vec3 getPosition() const { return mPosition; }

            ALuint getSourceID() const { return mSource; }

            ALint getState() const {
                ALint state;
                alGetSourcei(mSource, AL_SOURCE_STATE, &state);
                return state;
            }
    
        private:
            ALuint mSource;
            glm::vec3 mPosition;
    };
}