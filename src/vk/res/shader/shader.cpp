#include "shader.hpp"

#define MAX_DESCRIPTOR_TYPE_NAME_LENGTH 23

namespace Sierra::vlk {

    const std::vector<const char*> descriptorTypeNames = {
        "SAMPLER",
        "COMBINED_IMAGE_SAMPLER",
        "SAMPLED_IMAGE",
        "STORAGE_IMAGE",
        "UNIFORM_TEXEL_BUFFER",
        "STORAGE_TEXEL_BUFFER",
        "UNIFORM_BUFFER",
        "STORAGE_BUFFER",
        "UNIFORM_BUFFER_DYNAMIC",
        "STORAGE_BUFFER_DYNAMIC",
        "INPUT_ATTACHMENT"
    };

    Shader::Shader(): ctx(nullptr), module(VK_NULL_HANDLE), descriptorSizes() {

    }
    
    Shader::Shader(Context& ctx, std::string path): ctx(&ctx), module(VK_NULL_HANDLE), descriptorSizes() {
        createShader(path);
    }

    void Shader::readFile(std::string path, std::vector<char>& buff) {
        std::ifstream stream(path, std::ios::ate | std::ios::binary);

        if (!stream.is_open()) {
            char strBuff[128];
            snprintf(strBuff, sizeof(strBuff), "Failed to open a shader module for reading %s", path.c_str());

            throw new std::runtime_error(strBuff);
        }

        buff.resize(stream.tellg());
        stream.seekg(0);

        stream.read(buff.data(), buff.size());
    }

    void Shader::createShader(std::string path) {
        std::vector<char> buff;
        readFile(path, buff);

        parseShader(buff);

        VkShaderModuleCreateInfo moduleInfo{};
        moduleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

        moduleInfo.codeSize = buff.size();
        moduleInfo.pCode = (uint32_t*)buff.data();

        VK_ERR(vkCreateShaderModule(ctx->device->getDevice(), &moduleInfo, nullptr, &module));
    }

    void Shader::parseShader(std::vector<char>& buff) {
        for(int i = 0; i < buff.size(); i++) {
            if(buff[i] == '@')
                i += parseLine(&buff[i]);
        }
    }

    uint32_t Shader::parseLine(char* line) {
        char buf[64]; 

        size_t bindingOffset = 0;
        size_t endlOffset = seekEndl(line);

        bool match = false;
        for(uint32_t i = 0; i < MAX_DESCRIPTOR_TYPE_NAME_LENGTH; i++) {
            if(line[i + 2] != ' ') continue;

            memcpy(buf, line + 1, i-1);
            buf[i] = '\0';
            match = true;
            bindingOffset = i + 1;

            break;
        }

        if(!match) {
            ERROR("Error when reading descriptor type");
            ERRPRINTF("Offending line: %.*s\n", endlOffset, line);
            return endlOffset;
        }

        match = false;
        for(uint32_t i = 0; i < descriptorTypeNames.size(); i++) {
            if (strcmp(buf, descriptorTypeNames[i])) continue;

            descriptorSizes[i]++;
            match = true;
        }

        if (!match) {
            ERROR("Error when parsing descriptor type, unknown type: " << buf);
            ERRPRINTF("Offending line: %.*s\n", endlOffset, line);
            return endlOffset;
        }

        uint32_t binding = std::stoi(&line[bindingOffset]);

        LOG("BINDING:" << binding);
        return endlOffset;
    }

    uint32_t Shader::seekEndl(char* line) {
        uint32_t i = 0;
        for(; line[i] != '\n'; i++);

        return i;
    }

    std::vector<VkDescriptorSetLayoutBinding> Shader::getDescriptorBindings() {
        //TODO
        throw new std::runtime_error("TODO implement");
    }

    size_t* Shader::getDescriptorCounts() {
        return descriptorSizes;
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