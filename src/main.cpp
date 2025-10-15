#include "vk/vulkan.hpp"
#include "window/window.hpp"

#include "vk/res/shader/descriptor/descriptor_set.hpp"
#include "vk/rendering/graphics_pipeline/pipeline_layout.hpp"
#include "vk/rendering/graphics_pipeline/graphics_pipeline.hpp"
#include "vk/res/command_buffer/command_pool.hpp"
#include "vk/res/command_buffer/command_buffer.hpp"
#include "vk/res/mem/buffer/buffer.hpp"
#include "vk/res/mem/image/image.hpp"
#include "vk/res/mem/mem_loader.hpp"
#include "vk/sync/fence/fence.hpp"

#include "scene/tasks/manager.hpp"
#include "scene/loader/loadable_resource.hpp"

#include "io/panic/panic_handler.hpp"

using namespace Sierra;
using namespace vlk;
int main() {    
    //set up panic handler
    PanicHandler::init();

    try{
    
    Window window = Window("hehe", {1280, 720});
    Vulkan vulkan = Vulkan(window);

    TaskManager taskManager = TaskManager();
    MemLoader loader = MemLoader(vulkan.getContext());
    MemoryManager memManager = MemoryManager(vulkan.getContext());

    uint8_t dat[] = {1, 2, 3, 4, 5, 1, 2, 3, 4, 5, 1, 2, 3, 4, 5, 1, 2, 3, 4, 5};

    std::future<Buffer> devBuff = loader.createBuff(memManager, Buffer::Type::DEVICE_LOCAL, Buffer::Usage::VERTEX, dat, sizeof(dat));

    MemLoader::ImageInfo imageInfo{};
    imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    imageInfo.extent = {5, 1, 1};
    imageInfo.layout = VK_IMAGE_LAYOUT_GENERAL;
    imageInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT;

    std::future<Image> img = loader.createImage(memManager, imageInfo, dat, sizeof(dat));

    taskManager.addTasks(memManager.getTasks());
    taskManager.start();

    while(!taskManager.isFinished());

    } catch (std::runtime_error e) {
        std::cerr << e.what() << "\n";
    }

   /* vlk::Scene scene = vulkan.createScene();
    vlk::Shader shaderVert = vlk::Shader(vulkan.getContext(), "test_shaders/vk/tri.vert.spv", "test_shaders/vk/tri.vert");
    vlk::Shader shaderFrag = vlk::Shader(vulkan.getContext(), "test_shaders/vk/tri.frag.spv", "test_shaders/vk/tri.frag");

    auto descriptors = shaderVert.getDescriptors();

    std::array<size_t, SIERRA_VLK_DESCRIPTOR_TYPE_COUNT> sizes = {};
    std::vector<Descriptor*> descriptorPtrs = {};

    for(auto& descriptor : descriptors) {
        sizes[descriptor.getType()]++;
        descriptorPtrs.push_back(&descriptor);
    }

    vlk::DescriptorPool pool = vlk::DescriptorPool(vulkan.getContext(), sizes);

    vlk::DescriptorSet set = vlk::DescriptorSet(vulkan.getContext(), descriptorPtrs, pool);

    std::vector<VkPushConstantRange> ranges{};
    VkPipelineLayout layout = vlk::PipelineLayout::getLayout(vulkan.getContext(), set.getLayout(), ranges);

    VkAttachmentDescription attachmentDescription{};
    attachmentDescription.format = vulkan.getSwapchain().getSurfaceFormat().format;
    attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
    attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    vlk::RenderPass::CreateInfo renderPassInfo{};
    renderPassInfo.attachments = {attachmentDescription};
    renderPassInfo.dependencies = {};
    renderPassInfo.subpasses = {subpass};


    vlk::RenderPass renderPass = vlk::RenderPass(vulkan.getContext(), renderPassInfo);

    VkPipelineVertexInputStateCreateInfo inputState{};
    inputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    VkPipelineShaderStageCreateInfo vertexStage{};
    vertexStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertexStage.module = shaderVert.getShader();
    vertexStage.pName = "main";
    vertexStage.stage = VK_SHADER_STAGE_VERTEX_BIT;

    VkPipelineShaderStageCreateInfo fragStage{};
    fragStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragStage.module = shaderFrag.getShader();
    fragStage.pName = "main";
    fragStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;

    vlk::GraphicsPipeline::PipelineInfo pipelineInfo = {};
    pipelineInfo.renderPass = &renderPass;
    pipelineInfo.basePipeline = VK_NULL_HANDLE;
    pipelineInfo.inputState = inputState;
    pipelineInfo.layout = layout;
    pipelineInfo.subpass = 0;
    pipelineInfo.stages = {vertexStage, fragStage};
    
    VkPipeline pipeline = vlk::GraphicsPipeline::getPipeline(vulkan.getContext(), pipelineInfo);

    vlk::DescriptorLayout::destroy(vulkan.getContext());
    vlk::GraphicsPipeline::destroy(vulkan.getContext());
    vlk::PipelineLayout::destroy(vulkan.getContext());*/


}