#pragma once

#include <vulkan/vulkan.h>

#include "core/context.hpp"

namespace Sierra::vlk {
    class Fence {
        public:
            Fence();
            Fence(Context& context);

            Fence(Fence&) = delete;

            Fence(Fence&&);
            void operator=(Fence&&);

            ~Fence();

            VkFence getFence();
        private:
            void createFence();

            Context* context;

            VkFence fence;
    };
}
