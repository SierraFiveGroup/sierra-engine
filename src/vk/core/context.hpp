#pragma once

#include "instance/instance.hpp"
#include "device/device.hpp"

namespace Sierra::vlk {

    struct Context {
        Instance* instance;
        Device* device;
    };
}