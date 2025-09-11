#pragma once

#include <string>
#include <fstream>
#include <vector>
#include <stdexcept>

#include <vulkan/vulkan.h>

#include "vk/core/context.hpp"
#include "descriptor/descriptor.hpp"

//just notes so ion forget
//Shaders will have 4 descriptor rings-ish since thats the max amount of sets that can be bound at a time on a lot of devices
// r1: global state stuff i.e camera and maybe some general render info like resolution
// r2: scene stuff whatever user wants blah blah
// r3: draw call specific stuff 
// r4: reserved 

// should descriptors (and pools) be managed per object or per scene

namespace Sierra::vlk {
    class Shader {
        public:
            Shader();
            Shader(Context& ctx, std::string path);

            Shader(Shader&&);
            void operator=(Shader&&);

            ~Shader();

            VkShaderModule getShader();
            size_t* getDescriptorCounts();
        private:
            void readFile(std::string path, std::vector<char>& buff);
            void createShader(std::string path);

            VkShaderModule module;
            Context* ctx;

            std::vector<Descriptor> descriptors[SIERRA_VLK_DESCRIPTOR_TYPE_COUNT]; 
            size_t descriptorSizes[SIERRA_VLK_DESCRIPTOR_TYPE_COUNT];

    };
}