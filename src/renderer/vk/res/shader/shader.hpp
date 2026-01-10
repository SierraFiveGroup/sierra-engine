#pragma once

#include <string>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <cstring>
#include <unordered_map>
#include <list>

#include <vulkan/vulkan.h>

#include "core/context.hpp"
#include "descriptor/descriptor.hpp"

//just notes so ion forget
//Shaders will have 4 descriptor rings-ish since thats the max amount of sets that can be bound at a time on a lot of devices
// r1: global state stuff i.e camera and maybe some general render info like resolution
// r2: scene stuff whatever user wants blah blah
// r3: draw call specific stuff 
// r4: reserved 

// should descriptors (and pools) be managed per object or per scene

//above any descriptors in shaders there should be a commented "decoration" in the following format
//to help in the parsing
//@descriptor_type binding


namespace Sierra::vlk {
    class Shader {
        public:
            Shader();
            Shader(Context& ctx, std::string path, std::string sourcePath);

            Shader(Shader&&);
            void operator=(Shader&&);

            ~Shader();

            VkShaderModule getShader();

            std::vector<Descriptor>& getDescriptors();

        private:
            void readFile(std::string path, std::vector<char>& buff);
            void createShader(std::string path);

            void parseShader(std::string path);
            uint32_t parseLine(char* line);
            uint32_t seekEndl(char* line);

            VkShaderModule module;
            Context* ctx;

            std::vector<Descriptor> descriptors;
            std::unordered_map<std::string, Descriptor*> descriptorMap;
            std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
    };
}