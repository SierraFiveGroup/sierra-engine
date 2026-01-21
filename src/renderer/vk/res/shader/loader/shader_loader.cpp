#include "shader_loader.hpp"

namespace Sierra::vlk {

    ShaderLoader::ShaderLoader() {

    }

    ShaderLoader::ShaderLoader(Context& context, TaskManager& taskManager): context(&context), taskManager(&taskManager) {
        createPool();
    }

    std::future<ShaderLoader::ShaderPack> ShaderLoader::loadPack(GraphicsPipeline::PipelineInfo pipelineInfo, 
        Shader::Path vertPath, Shader::Path fragParth) {
        std::promise<ShaderLoader::ShaderPack> promise = {};
        std::future<ShaderLoader::ShaderPack> future = promise.get_future();

        std::shared_ptr<uint8_t> vertInputDat{};
        size_t attributesSize = pipelineInfo.inputState.vertexAttributeDescriptionCount * sizeof(VkVertexInputAttributeDescription);
        size_t bindingsSize = pipelineInfo.inputState.vertexBindingDescriptionCount * sizeof(VkVertexInputBindingDescription);

        vertInputDat.reset((uint8_t*)new uint8_t[attributesSize + bindingsSize]);

        memcpy(vertInputDat.get(), pipelineInfo.inputState.pVertexAttributeDescriptions, attributesSize);
        memcpy(vertInputDat.get() + attributesSize, pipelineInfo.inputState.pVertexBindingDescriptions, bindingsSize);

        std::shared_ptr<AsyncDat> asyncDat = std::make_shared<AsyncDat>(pipelineInfo, vertInputDat, context, std::move(promise),
         vertPath, fragParth, &tasks, &descriptorPool);

        uint32_t id = rand();
        Task task = Task(Task::Stage::INIT, id, ShaderLoader::loadPackTask, std::reinterpret_pointer_cast<uint8_t>(asyncDat));

        tasks[id] = asyncDat;

        taskManager->addTask(task);

        return future;
    }

    void ShaderLoader::loadPackTask(std::shared_ptr<uint8_t> asyncDatu8) {
        AsyncDat& asyncDat = *(AsyncDat*)asyncDatu8.get();
        ShaderPack pack = {};

        try {
            loadShader(asyncDat.context, &pack, asyncDat.vertPath, asyncDat.fragPath);
            createPipeline(asyncDat.context, &pack, asyncDat.pool, asyncDat.pipelineInfo, asyncDat.vertexInputData.get());
            asyncDat.shaderPack.set_value(std::move(pack));
        } catch(...) {
            asyncDat.shaderPack.set_exception(std::current_exception());
        }
    }

    void ShaderLoader::loadShader(Context* context, ShaderPack* shaderPack, Shader::Path vertPath, Shader::Path fragPath) {
        shaderPack->vertexShader = Shader(*context, vertPath);
        shaderPack->fragmentShader = Shader(*context, fragPath);
    }

    void ShaderLoader::createPipeline(Context* context, ShaderPack* shaderPack, DescriptorPool* descriptorPool, GraphicsPipeline::PipelineInfo& pipelineInfo, uint8_t* vertInputDat) {
        std::vector<Descriptor> descriptors = shaderPack->vertexShader.getDescriptors();
        descriptors.insert(descriptors.end(),
         shaderPack->fragmentShader.getDescriptors().begin(), shaderPack->fragmentShader.getDescriptors().end());

        std::vector<Descriptor*> descriptorPtrs;
        descriptorPtrs.reserve(shaderPack->vertexShader.getDescriptors().size()
         + shaderPack->fragmentShader.getDescriptors().size());


        for(Descriptor& descriptor : shaderPack->vertexShader.getDescriptors()) {
            descriptorPtrs.push_back(&descriptor);;
        }
        for(Descriptor& descriptor : shaderPack->fragmentShader.getDescriptors()) {
            descriptorPtrs.push_back(&descriptor);
        }

        DescriptorSet set = DescriptorSet(*context, descriptorPtrs, *descriptorPool);

        //the lion is not concerned with push constants right now
        std::vector<VkPushConstantRange> pushConstantRanges = {};

        VkPipelineShaderStageCreateInfo vertexStage{};
        vertexStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertexStage.module = shaderPack->vertexShader.getShader();
        vertexStage.pName = SIERRA_VLK_SHADER_ENTRY_NAME;
        vertexStage.stage = VK_SHADER_STAGE_VERTEX_BIT;

        VkPipelineShaderStageCreateInfo fragmentStage{};
        fragmentStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragmentStage.module = shaderPack->fragmentShader.getShader();
        fragmentStage.pName = SIERRA_VLK_SHADER_ENTRY_NAME;
        fragmentStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;

        pipelineInfo.layout = PipelineLayout::getLayout(*context, set.getLayout(), pushConstantRanges);
        pipelineInfo.stages = {vertexStage, fragmentStage};

        size_t attributesSize = pipelineInfo.inputState.vertexAttributeDescriptionCount * sizeof(VkVertexInputAttributeDescription);

        pipelineInfo.inputState.pVertexAttributeDescriptions = (VkVertexInputAttributeDescription*)vertInputDat;
        pipelineInfo.inputState.pVertexBindingDescriptions = (VkVertexInputBindingDescription*)(vertInputDat + attributesSize);

        shaderPack->pipeline = GraphicsPipeline::getPipeline(*context, pipelineInfo);
    }

    void ShaderLoader::createPool() {
        std::array<size_t, SIERRA_VLK_DESCRIPTOR_TYPE_COUNT> descriptorSizes;
        for(int i = 0; i < descriptorSizes.size(); i++) {
            descriptorSizes[i] = -1;
        }

        descriptorPool = DescriptorPool(*context, descriptorSizes);
    }

    void ShaderLoader::onCompleteCallback(Task task) {
        AsyncDat& asyncDat = *(AsyncDat*)task.getDat().get();

        asyncDat.tasks->erase(task.getID());
    }

}