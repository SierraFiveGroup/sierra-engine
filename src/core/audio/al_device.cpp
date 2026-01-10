#include "al_device.hpp"
#include "io/logging/logger.hpp"

namespace Sierra {
    ALDevice::ALDevice(const std::string &deviceName) {
        mDevice = alcOpenDevice(deviceName.empty() ? nullptr : deviceName.c_str());
        if (!mDevice) {
            ERROR("Failed to open audio device");
            return;
        }

        LOG("Using OpenAL device: " << alcGetString(mDevice, ALC_DEVICE_SPECIFIER));

        mContext = alcCreateContext(mDevice, nullptr);
        if (!mContext || !alcMakeContextCurrent(mContext)) {
            ERROR("Failed to create or make current OpenAL context");
            alcCloseDevice(mDevice);
            mDevice = nullptr;
            mContext = nullptr;
        }
    }

    ALDevice::~ALDevice() {
        if (mContext) {
            alcMakeContextCurrent(nullptr);
            alcDestroyContext(mContext);
        }

        if (mDevice) {
            alcCloseDevice(mDevice);
        }
    }

    bool ALDevice::isValid() const {
        return mDevice != nullptr && mContext != nullptr;
    }
}