#pragma once

#include "instance/instance.hpp"
#include "device/device.hpp"

namespace Sierra::vk {

    struct Context {
        Instance* instance;
        Device* device;
    };
}