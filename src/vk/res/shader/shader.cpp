#include "shader.hpp"

namespace Sierra::vlk {

    Shader::Shader(): ctx(nullptr), module(VK_NULL_HANDLE) {

    }
    
    Shader::Shader(Context& ctx, std::string path): ctx(&ctx), module(VK_NULL_HANDLE) {
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

        VkShaderModuleCreateInfo moduleInfo{};
        moduleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

        moduleInfo.codeSize = buff.size();
        moduleInfo.pCode = (uint32_t*)buff.data();

        VK_ERR(vkCreateShaderModule(ctx->device->getDevice(), &moduleInfo, nullptr, &module));
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