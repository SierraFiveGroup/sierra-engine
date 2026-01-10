#include "al_sound.hpp"

namespace Sierra {
    ALSound::ALSound(ALBuffer &buf)
        : mPosition(0.f, 0.f, 0.f)
    {
        alGenSources(1, &mSource);
        alSourcei(mSource, AL_BUFFER, buf.getID());
        alSource3f(mSource, AL_POSITION, 0.f, 0.f, 0.f); //Defualt position
    }

    ALSound::~ALSound() {
        alDeleteSources(1, &mSource);
    }

    void ALSound::play() {
        alSourcePlay(mSource);
    }

    void ALSound::pause() {
        alSourcePause(mSource);
    }

    void ALSound::stop() {
        alSourceStop(mSource);
    }

    void ALSound::setLooping(bool loop) {
        alSourcei(mSource, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
    }

    void ALSound::setGain(float gain) {
        alSourcef(mSource, AL_GAIN, gain);
    }

    void ALSound::setPosition(const glm::vec3 &pos) {
        mPosition = pos;
        alSource3f(mSource, AL_POSITION, pos.x, pos.y, pos.z);
    }
}