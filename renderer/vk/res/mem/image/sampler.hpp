#pragma once

#include <vulkan/vulkan.h>

#include "core/context.hpp"

namespace Sierra::vlk {
    class Sampler {
        public:
            struct Info {
                VkFilter filter; 
                VkSamplerMipmapMode mipmapMode;
                VkSamplerAddressMode addressMode;
                float lodBias;
                float minLod;
                float maxLod;
                bool anisotropy;
                float maxAnisotropy;
            };

            Sampler();
            Sampler(Context& context, Info info);

            Sampler(Sampler&) = delete;
            Sampler operator=(Sampler&) = delete;

            Sampler(Sampler&&);
            void operator=(Sampler&&);

            ~Sampler();

            VkSampler getSampler();
        private:
            void createSampler(Info& info);

            VkSampler sampler;
            Context* context;
    };
}