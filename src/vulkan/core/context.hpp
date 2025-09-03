#pragma once

namespace Sierra::vk {
    class Instance;
    class Device;

    struct Context {
        Instance* instance;
        Device* device;
    };
}