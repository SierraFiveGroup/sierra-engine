#pragma once

#include <string>

#include "vk/core/context.hpp"
#include "vk/res/shader/shader.hpp"
#include "vk/rendering/graphics_pipeline/graphics_pipeline.hpp"
#include "vk/res/shader/descriptor/descriptor_set.hpp"

namespace Sierra::vlk {
    class ShaderLoader {
        public:
            struct ShaderPack {
                Shader vertexShader;
                Shader fragmentShader;
                VkPipeline pipeline;
            };

        protected:
            struct AsyncDat;

            typedef std::unordered_map<uint32_t, std::shared_ptr<AsyncDat>> TasksData_T;

            struct AsyncDat {
                GraphicsPipeline::PipelineInfo pipelineInfo;
                std::shared_ptr<uint8_t> vertexInputData; //lifetimes 🥀
                Context* context;
                std::promise<ShaderPack> shaderPack;
                Shader::Path vertPath;
                Shader::Path fragPath;
                TasksData_T* tasks;
                DescriptorPool* pool;
            };


        public:

            ShaderLoader();
            ShaderLoader(Context& context, TaskManager& taskManager);

            ShaderLoader(ShaderLoader&) = delete;
            ShaderLoader(ShaderLoader&&) = delete;
            
            std::future<ShaderPack> loadPack(GraphicsPipeline::PipelineInfo pipelineInfo, Shader::Path vertPath, Shader::Path fragParth);



        private:
            void createPool();

            static void loadPackTask(std::shared_ptr<uint8_t> asyncDat);
            static void loadShader(Context* context, ShaderPack* shaderPack, Shader::Path vertPath, Shader::Path fragPath);
            static void createPipeline(Context* context, ShaderPack* shaderPack, DescriptorPool* descriptorPool, GraphicsPipeline::PipelineInfo& pipelineInfo, uint8_t* vertInputDat);

            static void onCompleteCallback(Task task);

            Context* context;
            TaskManager* taskManager;

            TasksData_T tasks;

            DescriptorPool descriptorPool;
    };
}