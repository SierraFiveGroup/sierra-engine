#pragma once

#include "util/utilities.hpp"
#include <AL/al.h>
#include <AL/alc.h>
#include <string>

namespace Sierra {
    //Handler for the OpenAL device, via which audio buffers are played.
    class ALDevice {
        public:
            ALDevice(const std::string &deviceName = "");
            DISABLE_COPY_AND_MOVE(ALDevice);

            ~ALDevice();

            bool isValid() const;
        
        private:
            ALCdevice *mDevice;
            ALCcontext *mContext;
    };
}