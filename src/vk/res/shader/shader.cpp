#include "shader.hpp"

#define MAX_DESCRIPTOR_TYPE_NAME_LENGTH 23

namespace Sierra::vlk {

    const std::vector<std::pair<const char*, uint32_t>> descriptorTypeNames = {
        {"SAMPLER", 0},
        {"COMBINED_IMAGE_SAMPLER", 1},
        {"SAMPLED_IMAGE", 2},
        {"STORAGE_IMAGE", 3},
        {"UNIFORM_TEXEL_BUFFER", 4},
        {"STORAGE_TEXEL_BUFFER", 5},
        {"UNIFORM_BUFFER", 6},
        {"STORAGE_BUFFER", 7},
        //"UNIFORM_BUFFER_DYNAMIC",
        //"STORAGE_BUFFER_DYNAMIC",
        //{"INPUT_ATTACHMENT", 10}
    };

    Shader::Shader(): ctx(nullptr), module(VK_NULL_HANDLE) {

    }
    
    Shader::Shader(Context& ctx, std::string path, std::string sourcePath): ctx(&ctx), module(VK_NULL_HANDLE) {
        createShader(path);
        parseShader(sourcePath);
    }

    void Shader::readFile(std::string path, std::vector<char>& buff) {
        std::ifstream stream(path, std::ios::ate | std::ios::binary);

        if (!stream.is_open()) {
            char strBuff[128];
            snprintf(strBuff, sizeof(strBuff), "Failed to open a shader module for reading %s", path.c_str());

            throw std::runtime_error(strBuff);
        }

        buff.resize(stream.tellg());
        stream.seekg(0);

        stream.read(buff.data(), buff.size());
    }

    void Shader::createShader(std::string path) {
        std::vector<char> buff;
        readFile(path, buff);

        VkShaderModuleCreateInfo moduleInfo{};
        moduleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

        moduleInfo.codeSize = buff.size();
        moduleInfo.pCode = (uint32_t*)buff.data();

        VK_ERR(vkCreateShaderModule(ctx->device->getDevice(), &moduleInfo, nullptr, &module));
    }

    void Shader::parseShader(std::string path) {
        std::vector<char> buff;
        readFile(path, buff);

        for(int i = 0; i < buff.size(); i++) {
            if(buff[i] == '@')
                i += parseLine(&buff[i]);
        }
    }

    uint32_t Shader::parseLine(char* line) { // TODO cleanup this ugly piece of shit
        char buf[64]; 

        size_t offset = 0;
        size_t endlOffset = seekEndl(line);
        bool match = false;

        
        /////// descriptor type
        VkDescriptorType type;

        for(uint32_t i = 0; i < MAX_DESCRIPTOR_TYPE_NAME_LENGTH; i++) {
            if(line[i] != ' ') continue;

            memcpy(buf, line + 1, i-1);
            buf[i] = '\0';
            match = true;
            offset = i + 1;

            break;
        }

        if(!match) {
            ERROR("Error when reading descriptor type");
            ERRPRINTF("Offending line: %.*s\n", endlOffset, line);
            return endlOffset;
        }

        match = false;
        for(uint32_t i = 0; i < descriptorTypeNames.size(); i++) {
            if (strcmp(buf, descriptorTypeNames[i].first)) continue;

            type = (VkDescriptorType)descriptorTypeNames[i].second;
            match = true;
        }

        if (!match) {
            ERROR("Error when parsing descriptor type, unknown type: " << buf);
            ERRPRINTF("Offending line: %.*s\n", endlOffset, line);
            return endlOffset;
        }
        /////

        ///// get binding
        uint32_t binding = std::stoi(&line[offset]);
        /////////


        /////// get name
        for(; line[offset] != ' '; offset++);
        offset++;

        memcpy(buf, line + offset, endlOffset - offset); 
        buf[endlOffset - offset] = '\0';
        ////////

        descriptors.emplace_back(std::string(buf), type);

        return endlOffset;
    }

    uint32_t Shader::seekEndl(char* line) {
        uint32_t i = 0;
        for(; line[i] != '\n'; i++);

        return i;
    }

    std::vector<VkDescriptorSetLayoutBinding> Shader::getDescriptorBindings() {
        //TODO
        throw std::runtime_error("TODO implement");
    }

    std::vector<Descriptor>& Shader::getDescriptors() {
        return descriptors;
    }

    VkShaderModule Shader::getShader() {
        return module;
    }

    Shader::Shader(Shader&& other) {
        module = other.module;

        other.module = VK_NULL_HANDLE;
    }

    void Shader::operator=(Shader&& other) {
        module = other.module;

        other.module = VK_NULL_HANDLE;
    }

    Shader::~Shader() {
        if(module) 
            vkDestroyShaderModule(ctx->device->getDevice(), module, nullptr);
    }
}