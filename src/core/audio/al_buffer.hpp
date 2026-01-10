#pragma once

#include "io/logging/logger.hpp"
#include "util/utilities.hpp"
#include <AL/al.h>
#include <string>
#include <iostream>
#include <vector>
#include <cstdint>

namespace Sierra {
    //Handler for the OpenAL audio buffers. Used for loading audio and playing it in ALSound.
    class ALBuffer {
        public:
            ALBuffer();
            DISABLE_COPY_AND_MOVE(ALBuffer);

            ~ALBuffer();

            bool loadFromFile(const std::string &filename);
            bool loadWAV(const std::string &filename);
            bool loadMP3(const std::string &filename);
            ALuint getID() const { return mBuffer; }
        
        private:
            ALuint mBuffer;
    };
}